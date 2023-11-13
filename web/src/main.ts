import { enableProdMode, importProvidersFrom } from '@angular/core';

import { environment } from './environments/environment';
import { AppComponent, customTooltipDefaults } from './app/app.component';
import { ColorCircleModule } from 'ngx-color/circle';
import { ColorPhotoshopModule } from 'ngx-color/photoshop';
import { MatNativeDateModule } from '@angular/material/core';
import { NgxMatTimepickerModule } from 'ngx-mat-timepicker';
import { MatRadioModule } from '@angular/material/radio';
import { MatInputModule } from '@angular/material/input';
import { MatTabsModule } from '@angular/material/tabs';
import { ColorHueModule } from 'ngx-color/hue';
import { MatCheckboxModule } from '@angular/material/checkbox';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { SaturationModule } from 'ngx-color';
import { ColorChromeModule } from 'ngx-color/chrome';
import { DragDropModule } from '@angular/cdk/drag-drop';
import { MatSelectModule } from '@angular/material/select';
import { MatSliderModule } from '@angular/material/slider';
import { MatSlideToggleModule } from '@angular/material/slide-toggle';
import { ColorGithubModule } from 'ngx-color/github';
import { MatToolbarModule } from '@angular/material/toolbar';
import { MatMenuModule } from '@angular/material/menu';
import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { WebsocketModule } from './app/services/websocket/websocket.module';
import { provideAnimations } from '@angular/platform-browser/animations';
import { BrowserModule, bootstrapApplication } from '@angular/platform-browser';
import { withInterceptorsFromDi, provideHttpClient } from '@angular/common/http';
import { MAT_DIALOG_DATA, MatDialogModule } from '@angular/material/dialog';
import { MAT_TOOLTIP_DEFAULT_OPTIONS, MatTooltipDefaultOptions, MatTooltipModule } from '@angular/material/tooltip';
import { MatDatepickerModule } from '@angular/material/datepicker';

if (environment.production) {
  enableProdMode();
}

bootstrapApplication(AppComponent, {
    providers: [
        importProvidersFrom(BrowserModule, WebsocketModule, MatIconModule, MatButtonModule, MatMenuModule, MatToolbarModule,
        ColorGithubModule, MatSlideToggleModule, MatTooltipModule, MatSliderModule, MatSelectModule, MatDialogModule, DragDropModule,
        ColorChromeModule, SaturationModule, FormsModule, MatCheckboxModule, ColorHueModule, MatTabsModule, MatInputModule, ReactiveFormsModule,
        MatRadioModule, NgxMatTimepickerModule, MatDatepickerModule, MatNativeDateModule, ColorPhotoshopModule, ColorCircleModule),
        MatDatepickerModule,
        { provide: MAT_TOOLTIP_DEFAULT_OPTIONS, useValue: customTooltipDefaults },
        { provide: MAT_DIALOG_DATA, useValue: [] },
        provideHttpClient(withInterceptorsFromDi()),
        provideAnimations()
    ]
})
  .then((ref) => {
    // Ensure Angular destroys itself on hot reloads.
    if ((window as { [key: string]: any })['ngRef']) {
      (window as { [key: string]: any })['ngRef'].destroy();
    }
    (window as { [key: string]: any })['ngRef']  = ref;
    // Otherwise, log the boot error
  })
  .catch(err => console.error(err));
