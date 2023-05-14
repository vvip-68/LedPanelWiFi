import {DragDropModule} from '@angular/cdk/drag-drop';
import {HttpClientModule} from '@angular/common/http';
import {NgModule} from '@angular/core';
import {FormsModule, ReactiveFormsModule} from '@angular/forms';
import {MatButtonModule} from '@angular/material/button';
import {MatCheckboxModule} from '@angular/material/checkbox';
import {MAT_DIALOG_DATA, MatDialogModule} from '@angular/material/dialog';
import {MatIconModule} from '@angular/material/icon';
import {MatMenuModule} from '@angular/material/menu';
import {MatSelectModule} from '@angular/material/select';
import {MatSlideToggleModule} from '@angular/material/slide-toggle';
import {MatSliderModule} from '@angular/material/slider';
import {MatToolbarModule} from '@angular/material/toolbar';
import {MAT_TOOLTIP_DEFAULT_OPTIONS, MatTooltipDefaultOptions, MatTooltipModule} from '@angular/material/tooltip';
import {BrowserModule} from '@angular/platform-browser';
import {BrowserAnimationsModule} from '@angular/platform-browser/animations';
import {SaturationModule} from 'ngx-color';
import {ColorChromeModule} from 'ngx-color/chrome';
import {ColorGithubModule} from 'ngx-color/github';
import {ColorHueModule} from 'ngx-color/hue';
import {AppComponent} from './app.component';
import {ActionComponent} from './components/action/action.component';
import {BrightnessSliderComponent} from './components/brightness-slider/brightness-slider.component';
import {ColorPickerComponent} from './components/color-picker/color-picker.component';
import {EffectParamsComponent} from './components/effect-params/effect-params.component';
import {EffectComponent} from './components/effect/effect.component';
import {WebsocketModule} from './services/websocket/websocket.module';
import {MatTabsModule} from "@angular/material/tabs";
import {TabEffectsComponent} from "./components/tabs/tab-effects/tab-effects.component";
import {TabTextsComponent} from "./components/tabs/tab-texts/tab-texts.component";
import {TabClockComponent} from "./components/tabs/tab-clock/tab-clock.component";
import {TabAlarmComponent} from "./components/tabs/tab-alarm/tab-alarm.component";
import {TabModesComponent} from "./components/tabs/tab-modes/tab-modes.component";
import {TabDrawComponent} from "./components/tabs/tab-draw/tab-draw.component";
import {TabGamesComponent} from "./components/tabs/tab-games/tab-games.component";
import {TabSetupComponent} from "./components/tabs/tab-setup/tab-setup.component";
import {TabMatrixComponent} from "./components/tabs/tab-matrix/tab-matrix.component";
import {TabOtherComponent} from "./components/tabs/tab-other/tab-other.component";
import {TabNetworkComponent} from "./components/tabs/tab-network/tab-network.component";
import {TabWeatherComponent} from "./components/tabs/tab-weather/tab-weather.component";
import {TabSynchComponent} from "./components/tabs/tab-synch/tab-synch.component";
import {MatInputModule} from "@angular/material/input";
import {InputRestrictionDirective} from "./directives/input-restrict.directive";
import {DisableControlDirective} from "./directives/disable-control.directive";
import {MatRadioModule} from "@angular/material/radio";
import {TabNetworkSsidComponent} from "./components/tabs/tab-network-ssid/tab-network-ssid.component";
import {TabNetworkApComponent} from "./components/tabs/tab-network-ap/tab-network-ap.component";
import {TabWiringComponent} from "./components/tabs/tab-wiring/tab-wiring.component";
import {PluggerComponent} from "./components/plugger/plugger.component";
import {ConfirmationDialogComponent} from "./components/confirmation-dialog/confirmation-dialog.component";
import {NgxMatTimepickerModule} from "ngx-mat-timepicker";
import {MatDatepickerModule} from "@angular/material/datepicker";
import {MatNativeDateModule} from "@angular/material/core";
import {TabTextsPanelComponent} from "./components/tabs/tab-texts-panel/tab-texts-panel.component";
import {TabTextsHelpComponent} from "./components/tabs/tab-texts-help/tab-texts-help.component";
import {ColorPhotoshopModule} from "ngx-color/photoshop";
import {ColorCircleModule} from "ngx-color/circle";

export const customTooltipDefaults: MatTooltipDefaultOptions = {
  showDelay: 1000,
  hideDelay: 100,
  touchendHideDelay: 100
};

@NgModule({
  declarations: [
    AppComponent,
    ColorPickerComponent,
    ActionComponent,
    BrightnessSliderComponent,
    EffectComponent,
    EffectParamsComponent,
    TabEffectsComponent,
    TabTextsComponent,
    TabClockComponent,
    TabAlarmComponent,
    TabModesComponent,
    TabDrawComponent,
    TabGamesComponent,
    TabSetupComponent,
    TabMatrixComponent,
    TabSynchComponent,
    TabNetworkComponent,
    TabWeatherComponent,
    TabOtherComponent,
    InputRestrictionDirective,
    DisableControlDirective,
    TabNetworkSsidComponent,
    TabNetworkApComponent,
    TabWiringComponent,
    PluggerComponent,
    ConfirmationDialogComponent,
    TabTextsPanelComponent,
    TabTextsHelpComponent
  ],
  imports: [
    HttpClientModule,
    BrowserModule,
    BrowserAnimationsModule,
    WebsocketModule,
    MatIconModule,
    MatButtonModule,
    MatMenuModule,
    MatToolbarModule,
    ColorGithubModule,
    MatSlideToggleModule,
    MatTooltipModule,
    MatSliderModule,
    MatSelectModule,
    MatDialogModule,
    DragDropModule,
    ColorChromeModule,
    SaturationModule,
    FormsModule,
    MatCheckboxModule,
    ColorHueModule,
    MatTabsModule,
    MatInputModule,
    ReactiveFormsModule,
    MatRadioModule,
    NgxMatTimepickerModule,
    MatDatepickerModule,
    MatNativeDateModule,
    ColorPhotoshopModule,
    ColorCircleModule
  ],
  providers: [
    MatDatepickerModule,
    {provide: MAT_TOOLTIP_DEFAULT_OPTIONS, useValue: customTooltipDefaults},
    {provide: MAT_DIALOG_DATA, useValue: []}
  ],
  exports: [
    InputRestrictionDirective
  ],
  bootstrap: [AppComponent]
})
export class AppModule {
}
