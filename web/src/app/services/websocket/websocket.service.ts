import {Injectable, OnDestroy} from '@angular/core';
import {BehaviorSubject, interval, Observable, Observer, Subject, Subscription, takeUntil, takeWhile} from 'rxjs';
import {distinctUntilChanged, filter, map, share} from 'rxjs/operators';
import {WebSocketSubject, WebSocketSubjectConfig} from 'rxjs/webSocket';
import {environment} from "../../../environments/environment";

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
    // @formatter:oт
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
export class WebsocketService implements IWebsocketService, OnDestroy {

  private readonly config: WebSocketSubjectConfig<IWsMessage<any>>;

  private websocketSub: Subscription;
  private statusSub: Subscription;

  // Observable для реконнекта по interval
  private reconnection$: Observable<number> | null = null;
  private websocket$: WebSocketSubject<IWsMessage<any>> | null = null;

  // сообщает, когда происходит коннект и реконнект
  private connection$: Observer<boolean> | null = null;

  // вспомогательный Observable для работы с подписками на сообщения
  private wsMessages$: Subject<IWsMessage<any>>;

  private url = environment.production ? `ws://${window.location.hostname}/ws` : 'ws://192.168.0.100/ws'; // 'ws://192.168.4.1/ws';

  private pingInterval = 5000;
  private reconnectInterval = 5000;
  private last_ping_time: number = Date.now();
  private sentPing = false;
  private gotPong = false;

  private webSocketSubscription!: Subscription;
  private destroy$ = new Subject();

  // -------------------------------------

  public firstConnect = true;
  public isConnected = false;
  public isConnected$ = new BehaviorSubject<boolean>(false);

  // статус соединения
  public status$: Observable<boolean>;

  // -------------------------------------

  constructor() {

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

    this.statusSub = this.status$
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
    this.websocketSub = this.wsMessages$
      .pipe(takeUntil(this.destroy$))
      .subscribe({
        next: (data: IWsMessage<any>) => {
          console.log(data); //+++!!!
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
  ngOnDestroy() {
    this.webSocketSubscription.unsubscribe();
    this.destroy$.next(true);
    this.destroy$.complete();
  }
}
