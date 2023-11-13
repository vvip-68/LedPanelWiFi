import {Component, ElementRef, Inject, OnInit, ViewChild} from '@angular/core';
import {ColorEvent, HSVA, RGBA} from "ngx-color";
import { MAT_DIALOG_DATA, MatDialogRef, MatDialogModule } from "@angular/material/dialog";
import {TinyColor} from "@ctrl/tinycolor";
import { MatButtonModule } from '@angular/material/button';
import { ColorHueModule } from 'ngx-color/hue';
import { ColorChromeModule } from 'ngx-color/chrome';

@Component({
    selector: 'app-color-picker',
    templateUrl: './color-picker.component.html',
    styleUrls: ['./color-picker.component.scss'],
    standalone: true,
    imports: [
        MatDialogModule,
        ColorChromeModule,
        ColorHueModule,
        MatButtonModule,
    ],
})
export class ColorPickerComponent implements OnInit {

  // Color picker component from here: https://github.com/scttcper/ngx-color

  @ViewChild('colorView', {static: true}) panel!: ElementRef;
  @ViewChild('palette', {static: true}) palette!: ElementRef;

  get color(): RGBA {
    return this._colorRGB;
  }

  set color(val: RGBA) {
    this._colorRGB = val;
    this.colorHSV = new TinyColor(val).toHsv();
    this.setViewColor();
  }

  private _colorRGB!: RGBA;
  colorHSV!: HSVA;

  constructor(public dialogRef: MatDialogRef<ColorPickerComponent>,
              @Inject(MAT_DIALOG_DATA) private data: any) {
  }

  ngOnInit(): void {
    this.color = <RGBA>this.data.color;
    const chrome = this.palette.nativeElement.querySelector('.chrome-body');
    if (chrome) {
      chrome.style.display = 'none';
    }
  }

  changeColor($event: ColorEvent) {
    this.color = $event.color.rgb;
  }

  setViewColor() {
    this.panel.nativeElement.style.backgroundColor = `rgba(${this.color.r}, ${this.color.g}, ${this.color.b}, ${this.color.a})`;
  }

  cancel() {
    this.dialogRef.close();
  }
}
