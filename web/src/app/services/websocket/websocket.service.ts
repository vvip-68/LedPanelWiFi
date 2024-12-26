import {Injectable, OnDestroy} from '@angular/core';
import {BehaviorSubject, catchError, interval, Observable, Observer, Subject, throwError as observableThrowError, Subscription, takeUntil, takeWhile, tap} from 'rxjs';
import {distinctUntilChanged, filter, map, share} from 'rxjs/operators';
import {WebSocketSubject, WebSocketSubjectConfig} from 'rxjs/webSocket';
import {environment} from "../../../environments/environment";
import {Base} from "../../components/base.class";
import {HttpClient, HttpHeaders} from "@angular/common/http";

export const WS = {
  // 'Топики' получения состояния из устройства
  ON: {
    // @formatter:off
    STATE:   'stt',   //- сообщения о текущем статусе параметров устройства - индивидуальный режим
    DATA:    'dta',	  // - отправка запрошенных данных клиенту, в ответ на полученную команду, например '$6 7
    ERROR:   'err',	  // - уведомления об ошибке клиенту
    ALARM:   'alm',	  // - сообщения о событиях будильника
    AMODE:   'amd',	  // - сообщения о наступлении событий авторежимов по времени
    WEATHER: 'wtr',	  // - сообщения о получении актуальной погоды
    TIME:    'tme',	  // - сообщения о синхронизации времени
    SDCARD:  'sdc',	  // - сообщения о событиях SD-карты (загрузка файла эффекта)
    TEXT:    'txt',	  // - сообщения о событиях бегущей строки - запуск, окончание
    E131:    'e131',  // - сообщения о событиях синхронизации между матрицами
    EDT:     'edt'	  // - сообщения c параметрами редактируемого эффекта
    // @formatter:on
  },
  // 'Топики' отправки команд в устройство
  SEND: {
    COMMAND: 'cmd'
  }
};

export interface IWebsocketService {
  on<T>(event: string): Observable<T>;

  send(event: string, data: any): void;

  sendText(text: string): void;

  status$: Observable<boolean>;
}

export interface IWsMessage<T> {
  e: string;
  d: T;
}

@Injectable({
  providedIn: 'root'
})
export class WebsocketService extends Base implements IWebsocketService, OnDestroy {

  public host: string = environment.production ? window.location.hostname : '192.168.0.100';

  private readonly config: WebSocketSubjectConfig<IWsMessage<any>>;

  // Observable для реконнекта по interval
  private reconnection$: Observable<number> | null = null;
  private websocket$: WebSocketSubject<IWsMessage<any>> | null = null;

  // сообщает, когда происходит коннект и реконнект
  private connection$: Observer<boolean> | null = null;

  // вспомогательный Observable для работы с подписками на сообщения
  private wsMessages$: Subject<IWsMessage<any>>;

  private url = `ws://${this.host}/ws`;

  private pingInterval = 30000;
  private reconnectInterval = 5000;
  private last_ping_time: number = Date.now();
  private sentPing = false;
  private gotPong = false;

  private webSocketSubscription!: Subscription;

  // -------------------------------------

  public firstConnect = true;
  public isConnected = false;
  public isConnected$ = new BehaviorSubject<boolean>(false);

  // статус соединения
  public status$: Observable<boolean>;

  // -------------------------------------

  constructor(private httpClient: HttpClient) {
    super();

    this.config = {
      url: this.url,
      closeObserver: {
        next: (event: CloseEvent) => {
          console.log('WebSocket disconnected!');
          this.websocket$ = null;
          this.connection$?.next(false);
          this.reconnect();
        }
      },
      openObserver: {
        next: (event: Event) => {
          console.log('WebSocket connected!');
          this.connection$?.next(true);
          this.ping();
          this.firstConnect = false;
        }
      }
    };

    // статус соединения
    this.status$ = new Observable<boolean>((observer) => {
      this.connection$ = observer;
    }).pipe(share(), distinctUntilChanged());

    this.status$
      .pipe(takeUntil(this.destroy$))
      .subscribe((isConnected) => {
        if (this.isConnected !== isConnected) {
          this.isConnected = isConnected;
          this.isConnected$.next(this.isConnected);
        }
        if (isConnected) {
          document.body.classList.remove('noscroll');
        } else {
          document.body.classList.add('noscroll');
        }
      });

    // говорим, что что-то пошло не так + проверка полученных в сокет данных
    this.wsMessages$ = new Subject<IWsMessage<any>>();
    this.wsMessages$
      .pipe(takeUntil(this.destroy$))
      .subscribe({
        next: (data: IWsMessage<any>) => {
          console.log(data);
        },
        error: (error: ErrorEvent) => {
          console.error('WebSocket error!', error);
          this.connection$?.next(false);
        }
      });

    // Интервал проверки соединения - отвечает ли сервер (ping-pong)
    interval(this.pingInterval)
      .pipe(takeUntil(this.destroy$))
      .subscribe(() => {
        if (this.isConnected) {
          if (this.sentPing && !this.gotPong && (Date.now() - this.last_ping_time) > this.pingInterval / 1000) {
            // Сбросить флаги
            this.sentPing = false;
            this.last_ping_time = Date.now();
            this.connection$?.next(false);
            this.disconnect();
          }
        }
        this.ping();
      });

    this.connect();
  }

  private connect(): void {
    this.websocket$ = new WebSocketSubject(this.config); // создаем
    // если есть сообщения, шлем их в дальше,
    // если нет, ожидаем
    // реконнектимся, если получили ошибку
    this.webSocketSubscription = this.websocket$
      .pipe(takeUntil(this.destroy$))
      .subscribe({
        next: (message: IWsMessage<any>) => this.wsMessages$.next(message),
        error: (error: Event) => {
          if (!this.websocket$) {
            // run reconnect if errors
            this.connection$?.next(false);
            this.reconnect();
          }
        }
      });
  }

  private disconnect(): void {
    this.webSocketSubscription.unsubscribe();
  }

  private reconnect(): void {
    // Создаем interval со значением из reconnectInterval
    this.reconnection$ = interval(this.reconnectInterval)
      .pipe(takeWhile(() => !this.websocket$));

    // Пытаемся подключиться пока не подключимся
    this.reconnection$
      .pipe(takeUntil(this.destroy$))
      .subscribe({
        next: () => this.connect(),
        complete: () => {
          // Subject complete if reconnect attempts ending
          this.reconnection$ = null;
          if (!this.websocket$) {
            this.wsMessages$.complete();
            this.connection$?.complete();
          }
        },
        error: (error) => {
          console.log(error);
        }
      });
  }

  /*
  * send ping message to client
  * */
  private ping() {
    if (this.websocket$) {
      this.websocket$?.next({e: 'cmd', d: '$6 7|FM|UP'});
      this.sentPing = true;
      this.gotPong = false;
      this.last_ping_time = Date.now();
    } else if (this.reconnection$ == null) {
      this.reconnect();
    }
  }

  /*
  * answer on ping received
  * */
  public pong() {
    this.sentPing = false;
    this.gotPong = false;
    if (!this.isConnected) {
      this.connection$?.next(true);
    }
  }

  /*
  * on message event
  * */
  public on = <T>(event: string): Observable<T> => {
    return this.wsMessages$.pipe(
      filter((message: IWsMessage<any>) => message.e === event),
      map((message: IWsMessage<any>) => message.d)
    );
  };

  /*
  * on message to server
  * */
  public send(event: string, data: any = {}): void {
    if (this.isConnected) {
      if (event === WS.SEND.COMMAND) {
        this.websocket$?.next({e: event, d: data});
      }
    }
  }

  /*
  * on send text command message to server
  * */
  public sendText(text: string): void {
    this.send(WS.SEND.COMMAND, text);
  }

  public getFile(fileName: string): Observable<any> {

    let contentType: string;
    // https://github.com/angular/angular/issues/18586
    const reForFilenameExtension = /(?:\.([^.]+))?$/;
    // @ts-ignore
    const ext = reForFilenameExtension.exec(fileName)[1];
    switch (ext) {
      case 'json':
        contentType = 'text/plain';
        break;
      //   https://stackoverflow.com/questions/25727306/request-header-field-access-control-allow-headers-is-not-allowed-by-access-contr
      case 'tar':
      case 'tgz':
        contentType = 'application/x-www-form-urlencoded; charset=UTF-8';
        break;
      case 'txt':
        contentType = 'text/plain';
        break;
      default:
        contentType = 'text/plain';
        break;
    }

    const headers = new HttpHeaders().set('content-type', contentType).set('Cache-Control', 'no-cache, no-store, must-revalidate, post-check=0, pre-check=0').set('Pragma', 'no-cache');
    const url = `http://${this.host}/${fileName}`;

    switch (ext) {
      case 'json':
        contentType = 'text/plain';
        return this.httpClient.get(url, { headers: headers, responseType: 'json' })
          .pipe(
            map(response => response),
            tap(response => this.logResponse('getFile', 'getFile()', url, '', '', response)),
            catchError(error => {
              return observableThrowError(error);
            }));

      case 'tar':
      case 'tgz':
        contentType = 'application/x-www-form-urlencoded; charset=UTF-8';
        return this.httpClient.get(url, { headers: headers, responseType: 'blob' })
          .pipe(
            map(response => response),
            tap(response => this.logResponse('getFile', 'getFile()', url, '', '', response)),
            catchError(error => {
              return observableThrowError(error);
            }));

      case 'txt':
        contentType = 'text/plain';
        return this.httpClient.get(url, { headers: headers, responseType: 'text' })
          .pipe(
            map(response => response),
            tap(response => this.logResponse('getFile', 'getFile()', url, '', '', response)),
            catchError(error => {
              return observableThrowError(error);
            }));

      default:
        contentType = 'text/plain';
        return this.httpClient.get(url, { headers: headers, responseType: 'json' })
          .pipe(
            map(response => response),
            tap(response => this.logResponse('getFile', 'getFile()', url, '', '', response)),
            catchError(error => {
              return observableThrowError(error);
            }));
    }

  }

  public logResponse(caller: Object | string, functionName: string, request: any, dataset: any, component: any, response: any) {
    const name = typeof caller === 'string' ? caller : caller.constructor.name;
    window.console.group(name);
    window.console.log(functionName + ' << \'' + dataset + '\' and \'' + component + '\'');
    window.console.log('request:', request);
    window.console.log('response:', response);
    window.console.groupEnd();
  }

  public handleError(caller: Object | string, msg: string, response?: any) {
    const name = typeof caller === 'string' ? caller : caller.constructor.name;
    window.console.group(name);
    window.console.error(msg, response);
    window.console.groupEnd();
  }

  public handleWarning(caller: Object | string, msg: string, warn?: any) {
    const name = typeof caller === 'string' ? caller : caller.constructor.name;
    window.console.group(name);
    window.console.warn(msg, warn);
    window.console.groupEnd();
  }

  public handleInfo(caller: Object | string, msg: string, info?: any) {
    const name = typeof caller === 'string' ? caller : caller.constructor.name;
    window.console.group(name);
    window.console.info(msg, info);
    window.console.groupEnd();
  }

  public handleTrace(caller: Object | string, msg: string, trace?: any) {
    const name = typeof caller === 'string' ? caller : caller.constructor.name;
    window.console.group(name);
    window.console.log(msg, trace);
    window.console.groupEnd();
  }

  override ngOnDestroy() {
    this.webSocketSubscription.unsubscribe();
    super.ngOnDestroy();
  }
}
