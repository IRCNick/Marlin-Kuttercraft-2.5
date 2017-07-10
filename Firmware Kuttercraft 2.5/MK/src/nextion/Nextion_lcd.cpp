/**
 * MK & MK4due 3D Printer Firmware
 *
 * Based on Marlin, Sprinter and grbl
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 * Copyright (C) 2013 - 2016 Alberto Cotronei @MagoKimbra
 * Copyright (C) 2017 Kuttercraft Kuttercraft@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * Nextion_lcd.cpp
 *
 * Copyright (c) 2014-2016 Alberto Cotronei @MagoKimbra
 *
 * Grbl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Grbl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Grbl. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../../base.h"

#if ENABLED(NEXTION)
  #include "Nextion_lcd.h"
  //#include "Nextion_gfx.h"
  #include "Nextion.h"

  bool NextionON        = false;
  bool CambioOff        = true;
  bool SalirPausaC      = true;
  //bool HolaMundo      = false;
  //bool lcd_hola() { return LCD_CLICKED; }
  uint8_t NextionPage   = 0;
  char buffer[100]      = {0};
  uint32_t slidermaxval = 20;
  char lcd_status_message[30] = WELCOME_MSG;
  uint8_t lcd_status_message_level = 0;
  static millis_t next_lcd_update_ms;
  //uint8_t Calibrar    = 0;
  #if ENABLED(SDSUPPORT)
    uint8_t SDstatus    = 0; // 0 SD not insert, 1 SD insert, 2 SD printing
    NexUpload Firmware(NEXTION_FIRMWARE_FILE, 57600);
  #endif

  /*
  #if ENABLED(NEXTION_GFX)
    GFX gfx = GFX(200, 190);
  #endif
  */
  // Page
  NexPage Pstart        = NexPage(0, 0, "presentacion");
  NexPage Pcontrol      = NexPage(1, 0, "control"); //Pinfo antes
  NexPage PMenu_Sd      = NexPage(13, 0, "menu_sd");
  //NexPage Pmenu         = NexPage(2, 0, "menu");
  //NexPage Photend       = NexPage(3, 0, "hotend");
  //NexPage Pcama         = NexPage(4, 0, "cama");
  //NexPage Pteclado_hot  = NexPage(5, 0, "teclado_hot");
  //NexPage Pteclado_cama = NexPage(6, 0, "teclado_cama");
  //NexPage Pfan          = NexPage(7, 0, "fan");
  //NexPage Pvelocidad    = NexPage(8, 0, "velocidad");
  //NexPage PMoverEje     = NexPage(9, 0, "mover_eje");
  // la pagina 10 es el brillo
  //NexPage PAju_Fila     = NexPage(11, 0, "aju_fila");
  //NexPage PCali_Asis    = NexPage(12, 0, "cali_asis");


  // Page 0 Start
  NexTimer startimer    = NexTimer(0,  1, "tm0");

  // Page 1 Info
  NexText InfoSet       = NexText(1, 4, "info18");
  NexText LedCoord1     = NexText(1,  5,  "icoord");
  NexText TimerEnd      = NexText(1,  6,  "Timer");
  NexTimer fantimer     = NexTimer(1, 7, "tm0");
  NexVar FanSp          = NexVar(1, 8, "fan_speed");
  NexText HotendAct     = NexText(1, 9, "tem_act");
  NexText HotendObj     = NexText(1, 10, "tem_obj");
  NexText CamadAct      = NexText(1, 11, "tem_act_bed");
  NexText CamaObj       = NexText(1, 12, "tem_obj_bed");
  NexPicture FanFoto    = NexPicture(1, 13, "fan");
  NexText MostrarFan    = NexText(1, 14, "fan_val");//feedrate_percentage velocidad
  NexText VeloTxt       = NexText(1, 19, "velocidad");
  NexVar SD             = NexVar(1, 22, "sd");
  NexPicture SdBot      = NexPicture(1,  23, "sd_boton");
  NexPicture NPlay      = NexPicture(1,  24, "pausa");//SdRest
  NexPicture SdRest     = NexPicture(1,  26, "sdrest");
  NexVar ConGcode       = NexVar(1, 27, "gcode");
  NexProgressBar sdbar  = NexProgressBar(1, 28, "progres");

  //NexText Hotend1       = NexText(1,  2,  "t1");//temperatura hotend1
  //NexText Hotend21      = NexText(1,  5,  "h2");//texto Hotend2 o cama
  //NexText Hotend2       = NexText(1,  6,  "t2");//temperatura Hotend2
  //NexText TextoSt       = NexText(1, 4, "info18"); //Hotend0 //LedStatus
  /*NexPicture MSD1       = NexPicture(1, 10, "p1");*/
  //NexPicture Hend0      = NexPicture(1, 12, "p3");
  //NexHotspot hot0       = NexHotspot(1, 13, "hot0");
  //NexPicture Hend1      = NexPicture(1, 14, "p4");
  //NexHotspot hot1       = NexHotspot(1, 15, "hot1");
  //NexPicture Hend2      = NexPicture(1, 16, "p5");
  //NexHotspot hot2       = NexHotspot(1, 17, "hot2");

  //NexVar Hotend         = NexVar(1, 19, "he");//Variables de estrusor 0,1,2,3
  //NexVar Bed            = NexVar(1, 20, "bed");//cama 0,1

  //NexButton Boton       = NexButton(1, 31, "bot");

  //NexProgressBar sdbar  = NexProgressBar(1, 23, "j0");
  //NexPicture NPlay      = NexPicture(1, 24, "p7");
  //NexPicture NStop      = NexPicture(1, 25, "p8");
  //NexVar SD             = NexVar(1, 26, "sd");
  //NexVar RFID           = NexVar(1, 27, "rfid");
  //NexPicture Speedpic   = NexPicture(1, 28, "p9");
  //NexVar VSpeed         = NexVar(1, 29, "vspeed");
  //NexVar Extruder       = NexVar(1, 30, "extruder");//parece inutil

  // Page 2 Temp
  //NexText set0          = NexText(2,  2,  "set0");
  //NexHotspot m11        = NexHotspot(2, 14, "m11");
  //NexHotspot tup        = NexHotspot(2, 15, "tup");
  //NexHotspot tdown      = NexHotspot(2, 16, "tdown");
  //NexVar set1           = NexVar(2, 17, "set1");

  //pagina 2 Menu
  NexButton Parar    = NexButton(2, 2, "acep");

  // Page 3 Control de Temperatura Extrusor
  //NexButton SetHoten    = NexButton(3, 1, "set");
  //NexText TemObjMod     = NexText(3, 8, "tem_obj_mod");
  //NexButton BotPla      = NexButton(3, 2, "pla");
  //NexButton BotAbs      = NexButton(3, 3, "abs");
  //NexButton BotFle      = NexButton(3, 4, "flex");
  //NexButton BotNay      = NexButton(3, 5, "nylon");
  //NexText TextPru     = NexText(3, 9, "t0");
  //NexPicture MSD3       = NexPicture(3,  2, "p1");
  //NexPicture Exit1      = NexPicture(3,  4, "p3");

  // Page 4 Control de Temperatura cama
  //NexButton SetCama        = NexButton(4, 1, "set");
  //NexText TemBedMod        = NexText(4, 8, "tem_bed_mod");
  //NexButton BotBedPla      = NexButton(4, 2, "pla");
  //NexButton BotBedAbs      = NexButton(4, 3, "abs");
  //NexButton BotBedFle      = NexButton(4, 4, "flex");
  //NexButton BotBedNay      = NexButton(4, 5, "nylon");
  //NexText TextPru        = NexText(3, 9, "t0");
  //NexPicture MSD3       = NexPicture(4,  2, "p1");
  //NexPicture Exit1      = NexPicture(4,  4, "p3");

  //Page 5 teclado Hotend
  //NexText TemObjTec        = NexText(5, 1, "tem_obj_mod");
  //NexButton SetTecHot      = NexButton(5, 3, "set");
  //NexVar TempVarTec        = NexVar(5, 16, "temp_val");

  //page 6 teclado cama
  //NexText TemBedObjTec     = NexText(6, 1, "tem_obj_mod");
  //NexButton SetTecBed      = NexButton(6, 3, "set");

  //pagina 7 ajustes de fan
  NexVar SetVar            = NexVar(7, 8, "mgcode");
  //NexButton SetFan         = NexButton(7, 3, "set");
  //NexText FanText          = NexText(7, 2, "fan_prin");
  //NexButton SetOnFan       = NexButton(7, 4, "on");
  //NexButton SetOffFan      = NexButton(7, 5, "off");//fan_num
  //NexVar FanVar            = NexVar(7, 7, "fan_num");

  //pagina 8 velocidad
  //NexButton SetVelo        = NexButton(8, 3, "set");
  //NexText VeloVar          = NexText(8, 16, "tem_obj_mod");
  //
  //pagina 9 Mover Eje
  //NexButton BotIzq         = NexButton(9, 9, "izq_bot");
  //NexButton BotDer         = NexButton(9, 11, "der_bot");
  NexButton BotHome        = NexButton(9, 10, "home_bot");
  NexVar ComandoM          = NexVar(9, 14, "total");
  NexText CoordText        = NexText(9, 15, "coord");

  //pagina 10 brillo

  //pagina 11 cambiar filamento
  NexText TemFil            = NexText(11,  5, "temp");// relplasa a reanudar y a cargar
  NexText TemFil2           = NexText(11,  9, "temp2");
  //NexVar VarGcode           = NexVar(11, 12, "gcode");
  //NexButton Cambiar         = NexButton(11,  16, "acep");
  //NexVar PausAFila          = NexVar(11, 17, "sd");
  //NexButton Reanudar        = NexButton(11, 21, "reanudar");
  //NexButton Carga           = NexButton(11, 22, "cargar");

  //pagina 12 Calibrar
  NexButton BotSetCal       = NexButton(12, 7, "acep_cali");
  NexButton MasHome         = NexButton(12, 10, "mas");
  NexVar  GcodeC            = NexVar(12, 13, "home_mas");
  NexVar  CoordC            = NexVar(12, 12, "punto");

  //pagina 13 Menu Sd

  NexSlider sdlist        = NexSlider(13, 1, "lista");

  //subir y vajar
  NexHotspot Mas          = NexHotspot(13, 3, "mas");
  NexHotspot Menos        = NexHotspot(13, 4, "menos");

  //Botones sd
  NexPicture Volver       = NexPicture(13, 5, "volver");
  NexButton Expulsar      = NexButton(13, 6, "expulsar");

  //lista de carpetas
  NexPicture Carpeta0     = NexPicture(13,  7, "carp0");
  NexPicture Carpeta1     = NexPicture(13,  8, "carp1");
  NexPicture Carpeta2     = NexPicture(13,  9, "carp2");
  NexPicture Carpeta3     = NexPicture(13,  10, "carp3");
  NexPicture Carpeta4     = NexPicture(13,  11, "carp4");
  NexPicture Carpeta5     = NexPicture(13,  12, "carp5");

  //lista de nombres
  NexText TextSd0         = NexText(13,  13, "txt0");
  NexText TextSd1         = NexText(13,  14, "txt1");
  NexText TextSd2         = NexText(13,  15, "txt2");
  NexText TextSd3         = NexText(13,  16, "txt3");
  NexText TextSd4         = NexText(13,  17, "txt4");
  NexText TextSd5         = NexText(13,  18, "txt5");

  //Nombre de carpetas
  NexText SdTexto         = NexText(13, 19,  "sd_txt");

  //pagina 14 auto leving
  NexText offsetTex         = NexText(14,  5, "offset");

  // Page 4 SDCard
  //NexSlider sdlist      = NexSlider(4, 1, "h0");
  //NexText sdrow0        = NexText(4,  3,  "t0");
  //NexText sdrow1        = NexText(4,  4,  "t1");
  //NexText sdrow2        = NexText(4,  5,  "t2");
  //NexText sdrow3        = NexText(4,  6,  "t3");
  //NexText sdrow4        = NexText(4,  7,  "t4");
  //NexText sdrow5        = NexText(4,  8,  "t5");
  //NexPicture Folder0    = NexPicture(4,  9, "p0");
  //NexPicture Folder1    = NexPicture(4, 10, "p1");
  ///NexPicture Folder2    = NexPicture(4, 11, "p2");
  //NexPicture Folder3    = NexPicture(4, 12, "p3");
  //NexPicture Folder4    = NexPicture(4, 13, "p4");
  //NexPicture Folder5    = NexPicture(4, 14, "p5");
  //NexPicture Folderup   = NexPicture(4, 15, "p6");
  //NexPicture Exit2      = NexPicture(4, 16, "p7");
  //NexText sdfolder      = NexText(4, 17,  "sdfolder");
  //NexHotspot ScrollUp   = NexHotspot(4, 21, "m0");
  //NexHotspot ScrollDown = NexHotspot(4, 22, "m1");


  // Page 5 Setup
  //NexPicture MSD5       = NexPicture(5,  2, "p1");
  //NexPicture Exit3      = NexPicture(5,  4, "p3");

  // Page 6 Move
  //NexPicture MSD6       = NexPicture(6,  2, "p1");
  //NexPicture XYHome     = NexPicture(6,  5, "p4");
  //NexPicture XYUp       = NexPicture(6,  6, "p5");
  //NexPicture XYRight    = NexPicture(6,  7, "p6");
  //NexPicture XYDown     = NexPicture(6,  8, "p7");
  //NexPicture XYLeft     = NexPicture(6,  9, "p8");
  //NexPicture ZHome      = NexPicture(6, 10, "p9");
  //NexPicture ZUp        = NexPicture(6, 11, "p10");
  //NexPicture ZDown      = NexPicture(6, 12, "p11");
  //NexVar movecmd        = NexVar(6, 18, "vacmd");
  //NexText LedCoord6     = NexText(6, 19, "mcoord");

  // Page 7 Speed
  //NexPicture SpeedOk    = NexPicture(7, 2,  "p0");

  // Page 8 Gcode
  //NexText Tgcode        = NexText(8, 1, "tgcode");
  //NexButton Benter      = NexButton(8, 41, "benter");

  // Page 9 Rfid
  //NexPicture MSD9       = NexPicture(9, 2,  "p1");
  //NexButton Rfid0       = NexButton(9,  5,  "b0");
  //NexButton Rfid1       = NexButton(9,  6,  "b1");
  //NexButton Rfid2       = NexButton(9,  7,  "b2");
  //NexButton Rfid3       = NexButton(9,  8,  "b3");
  //NexButton Rfid4       = NexButton(9,  9,  "b4");
  //NexButton Rfid5       = NexButton(9,  10, "b5");
  //NexDSButton RfidR     = NexDSButton(9,11, "bt0");
  //NexText RfidText      = NexText(9,    13, "t0");

  NexTouch *nex_listen_list[] =
  {
    &Pstart,
    //&SetHoten,
    //&SetCama,
    //&SetTecHot,
    //&SetTecBed,
    //&SetFan,
    &SetVar,
    &MostrarFan,
    &TimerEnd,
    &Parar,
    //&TemObjMod,
    //&BotIzq,
    //&BotDer,
    &BotHome,
    &ComandoM,
    //&SetVelo,
    //&VeloVar,
    &sdlist,
    &TextSd0,
    &TextSd1,
    &TextSd2,
    &TextSd3,
    &TextSd4,
    &TextSd5,
    &Volver,
    &Expulsar,
    &Mas,
    &Menos,
    &SdBot,
    &SdRest,
    &NPlay,
    &TemFil,
    &BotSetCal,
    &MasHome,
    &TemFil2,
    //&Cambiar,
    //&Reanudar,
    //&Carga,
    &offsetTex,
    //&GcodeCal,
    //&BotPla,
    //&BotAbs,
    //&BotFle,
    //&BotNay,
    //&BotBedPla,
    //&BotBedAbs,
    //&BotBedFle,
    //&BotBedNay,
    //&SetOnFan,
    //&SetOffFan,
    //&MSD1,
    //&MSD3,
    //&MSD5,
    //&MSD6,
    //&MSD9,
    //&TextPru,
    //&Fanpic,
    //&Speedpic,
    //&VarGcode,
    //&NStop,
    //&hot0,
    //&hot1,
    //&hot2,
    //&m11,
    //&tup,
    //&tdown,
    //&sdlist,
    //&ScrollUp,
    //&ScrollDown,
    //&sdrow0,
    //&sdrow1,
    //&sdrow2,
    //&sdrow3,
    //&sdrow4,
    //&sdrow5,
    //&Folderup,
    //&Exit1,
    //&Exit2,
    //&Exit3,
    //&XYHome,
    //&XYUp,
    //&XYRight,
    //&XYDown,
    //&XYLeft,
    //&ZHome,
    //&ZUp,
    //&ZDown,
    //&SpeedOk,
    //&Benter,
    //&Boton,
    //&Rfid0,
    //&Rfid1,
    //&Rfid2,
    //&Rfid3,
    //&Rfid4,
    //&Rfid5,
    NULL
  };

  NexText *hotend_list[] =
  {
    &HotendObj,
    &HotendAct,
    &CamaObj,
    &CamadAct,
    &MostrarFan,
    &VeloTxt,
    &TemFil,
    &TemFil2,
    NULL
  };
  NexText *row_list[] =
  {
    &TextSd0,
    &TextSd1,
    &TextSd2,
    &TextSd3,
    &TextSd4,
    &TextSd5,
    NULL
  };
  NexPicture *folder_list[] =
  {
    &Carpeta0,
    &Carpeta1,
    &Carpeta2,
    &Carpeta3,
    &Carpeta4,
    &Carpeta5,
    NULL
  };

  void setpageInfo() {
    if (NextionPage == 0) {
      Pcontrol.show();
      /*
      #if HAS(TEMP_0) // se setea  el valor de la var Hotend
        Hotend.setValue(1);
      #endif
      #if HAS(TEMP_1)
        Hotend.setValue(2);
      #endif
      #if HAS(TEMP_2)
        Hotend.setValue(3);
      #elif HAS(TEMP_BED)
        Bed.setValue(1);
        Hotend21.setText("BED");
      #endif
      */

      //Extruder.setValue(EXTRUDERS);// parece inutil
      //VSpeed.setValue(100);
      /*
      #if HAS(FAN)
        Fan.setValue(1);
      #endif
      */
      /*
      #if ENABLED(RFID_MODULE)
        RFID.setValue(1);
      #endif
      */
      //punto de interes
      lcd_setstatus(lcd_status_message);
    }

    Pcontrol.show();
    /*
    #if ENABLED(NEXTION_GFX)
      #if MECH(DELTA)
        gfx_clear((X_MAX_POS) * 2, (Y_MAX_POS) * 2, Z_MAX_POS);
      #else
        gfx_clear(X_MAX_POS, Y_MAX_POS, Z_MAX_POS);
      #endif
    #endif
    */
  }

  #if ENABLED(SDSUPPORT)
    void printrowsd(uint8_t row, const bool folder, const char* filename) {
      if (folder) {
        folder_list[row]->setShow();
        row_list[row]->attachPop(sdfolderPopCallback, row_list[row]);
      } else if (filename == "") {
        folder_list[row]->setHide();
        row_list[row]->detachPop();
      } else {
        folder_list[row]->setHide();
        row_list[row]->attachPop(sdfilePopCallback, row_list[row]);
      }
      row_list[row]->setText(filename);
    }

    static void setrowsdcard(uint32_t number = 0) {
      uint16_t fileCnt = card.getnrfilenames();
      uint32_t i = 0;
      card.getWorkDirName();

      if (fullName[0] != '/') {
        Volver.setShow();
        Volver.attachPop(sdfolderUpPopCallback);
        SdTexto.setText(fullName);
      } else {
        Volver.detachPop();
        Volver.setHide();
        SdTexto.setText("");
      }

      for (uint8_t row = 0; row < 6; row++) {
        i = row + number;
        if (i < fileCnt) {
          card.getfilename(i);
          printrowsd(row, card.filenameIsDir, fullName);
        } else {
          printrowsd(row, false, "");
        }
      }
      //sendCommand("ref 0");
    }

    static void menu_action_sdfile(const char* filename) {
      char cmd[50];
      char* c;
      sprintf_P(cmd, PSTR("M23 %s"), filename);
      for(c = &cmd[4]; *c; c++) *c = tolower(*c);
      enqueue_and_echo_command(cmd);
      enqueue_and_echo_commands_P(PSTR("M24"));
      setpageInfo();
    }


    static void menu_action_sddirectory(const char* filename) {
      card.chdir(filename);
      setpageSDPopCallback(&SdBot);
    }

    void otraFuncion(){ return false; }

    void setpageSDPopCallback(void *ptr) {
      PMenu_Sd.show();
      uint16_t fileCnt = card.getnrfilenames();

      if (fileCnt <= 6)
        slidermaxval = 0;
      else
        slidermaxval  = fileCnt - 6;

      //uint16_t hig = 210 - slidermaxval * 10;
      uint16_t hig = 234 - slidermaxval * 10;
      if (hig < 10) hig = 10;

      sdlist.setHigVal(hig);
      sdlist.setMaxVal(slidermaxval);
      sdlist.setValue(slidermaxval);
      //sendCommand("ref 0");

      setrowsdcard();
    }

    void sdlistPopCallback(void *ptr) {
      uint32_t number = 0;
      sdlist.getValue(&number);
      number = slidermaxval - number;
      setrowsdcard(number);
    }

    void sdfilePopCallback(void *ptr) {
      memset(buffer, 0, sizeof(buffer));

      if (ptr == &TextSd0)
        TextSd0.getText(buffer, sizeof(buffer));
      else if (ptr == &TextSd1)
        TextSd1.getText(buffer, sizeof(buffer));
      else if (ptr == &TextSd2)
        TextSd2.getText(buffer, sizeof(buffer));
      else if (ptr == &TextSd3)
        TextSd3.getText(buffer, sizeof(buffer));
      else if (ptr == &TextSd4)
        TextSd4.getText(buffer, sizeof(buffer));
      else if (ptr == &TextSd5)
        TextSd5.getText(buffer, sizeof(buffer));

      menu_action_sdfile(buffer);
    }


    void sdfolderPopCallback(void *ptr) {
      memset(buffer, 0, sizeof(buffer));

      if (ptr == &TextSd0)
        TextSd0.getText(buffer, sizeof(buffer));
      else if (ptr == &TextSd0)
        TextSd1.getText(buffer, sizeof(buffer));
      else if (ptr == &TextSd2)
        TextSd2.getText(buffer, sizeof(buffer));
      else if (ptr == &TextSd3)
        TextSd3.getText(buffer, sizeof(buffer));
      else if (ptr == &TextSd4)
        TextSd4.getText(buffer, sizeof(buffer));
      else if (ptr == &TextSd5)
        TextSd5.getText(buffer, sizeof(buffer));

      menu_action_sddirectory(buffer);
    }


    void sdfolderUpPopCallback(void *ptr) {
      card.updir();
      setpageSDPopCallback(&SdBot);
    }

    /*
    void UploadNewFirmware() {
      if(IS_SD_INSERTED || card.cardOK) {
        Firmware.startUpload();
        nexSerial.end();
        lcd_init();
      }
    }
    */
  #endif

  /*
  #if ENABLED(RFID_MODULE)
    void rfidPopCallback(void *ptr) {

      memset(buffer, 0, sizeof(buffer));
      String temp = "M522 ";
      uint32_t Rfid_read = 0;
      RfidR.getValue(&Rfid_read);

      if (ptr == &Rfid0)
        temp += "T0 ";
      else if (ptr == &Rfid1)
        temp += "T1 ";
      else if (ptr == &Rfid2)
        temp += "T2 ";
      else if (ptr == &Rfid3)
        temp += "T3 ";
      else if (ptr == &Rfid4)
        temp += "T4 ";
      else if (ptr == &Rfid5)
        temp += "T5 ";

      if(Rfid_read)
        temp += "R";
      else
        temp += "W";

      temp.toCharArray(buffer, sizeof(buffer));
      enqueue_and_echo_commands_P(buffer);
    }

    void rfid_setText(const char* message, uint32_t color) {
      char Rfid_status_message[25];
      strncpy(Rfid_status_message, message, 30);
      RfidText.setColor(color);
      RfidText.setText(Rfid_status_message);
    }
  #endif
  */
  void ExitPopCallback(void *ptr) {
    setpageInfo();
  }

  void PstartPopCallback(void *ptr) {
    setpageInfo();
  }

  /*
  void hotPopCallback(void *ptr) {
    Ptemp.show();//muestra teclado
    memset(buffer, 0, sizeof(buffer));

    if (ptr == &hot0) {
      if (degTargetHotend(0) != 0) {
        itoa(degTargetHotend(0), buffer, 10);
      }
      set1.setText("M104 T0 S");
    }


    if (ptr == &hot1) {
      if (degTargetHotend(1) != 0) {
        itoa(degTargetHotend(1), buffer, 10);
      }
      set1.setText("M104 T1 S");
    }


    #if HAS_TEMP_2//ajustan temperatura extrusor
      if (ptr == &hot2) {
        if (degTargetHotend(2) != 0) {
          itoa(degTargetHotend(2), buffer, 10);
        }
        set1.setText("M104 T2 S");
      }
    #elif HAS_TEMP_BED//ajustan temperatura cama
      if (ptr == &hot2) {
        if (degTargetBed() != 0) {
          itoa(degTargetBed(), buffer, 10);
        }
        set1.setText("M140 S");
      }
    #endif

    //set0.setText(buffer);
  }
  */
  /*
  void setHotenPopCallback(void *ptr) {

    memset(buffer, 0, sizeof(buffer));
    strcat(buffer, """M104 S112""");
    TextPru.setText(buffer);
    enqueue_and_echo_commands_P(buffer);
    //enqueue_and_echo_commands_P(PSTR("M104 S111"));

  }
  */
  //punto de interes
  // no sirve (ya funciona)
  //Se encarga de cambiar la temperatura al la del slider
  /*
  void setHotenPopCallback() {
    //----Inicio un buffer
    memset(buffer, 0, sizeof(buffer));
    TemObjMod.getText(buffer, sizeof(buffer));
    //sprintf_P(cmd, PSTR("M104 S%s"), TemObjMod.getText(cmd););
    //for(c = &cmd[4]; *c; c++) *c = tolower(*c);
    //enqueue_and_echo_command(buffer);
    //TextPru.setText(buffer);
    //enqueue_and_echo_commands_P("M104 S44");
    //-------------------------------------------------
    //----Inicio una cadena llama
    char cmd[30];
    char* c;
    //----inicializa la cadena cmd: con "M104 S".
    //----s% pemite unir la cadena cmd con buffer
    sprintf_P(cmd, PSTR("M104 S%s"), buffer);
    //for(c = &cmd[4]; *c; c++) *c = tolower(*c);
    //enqueue_and_echo_command(cmd);
    //enqueue_and_echo_commands_P("M104 S44");
    //----Envia el comando ya formado
    enqueue_and_echo_command(cmd);
    //----Actualiza la temperatura del panel de control
    //Pcontrol.show();            //nuevo protocolo
    //HotendObj.setText(buffer);  //nuevo protocolo

    //mostrarTempHot(0, degTargetHotend(0));
    //actualizar();
    //TextPru.setText(cmd);
    //mostrarTempHot(0, degTargetHotend(0));
  }
  */
  //slider cama
  /*
  void setCamaPopCallback() {
    memset(buffer, 0, sizeof(buffer));
    TemBedMod.getText(buffer, sizeof(buffer));
      char cmd[30];
      char* c;
      sprintf_P(cmd, PSTR("M140 S%s"), buffer);
      enqueue_and_echo_command(cmd);
      Pcontrol.show();
      CamaObj.setText(buffer);
  }
  */
  //teclado hotend
  /*
  void setTecHotPopCallback() {
    memset(buffer, 0, sizeof(buffer));
    TemObjTec.getText(buffer, sizeof(buffer));
      char cmd[30];
      char* c;
      sprintf_P(cmd, PSTR("M104 S%s"), buffer);
      enqueue_and_echo_command(cmd);
      //nuevo protocolo
      //Pcontrol.show();
      //HotendObj.setText(buffer);
  }
  */
  //teclado cama
  /*
  //cama
  void setTecBedPopCallback() {
    memset(buffer, 0, sizeof(buffer));
    TemBedObjTec.getText(buffer, sizeof(buffer));
      char cmd[30];
      char* c;
      sprintf_P(cmd, PSTR("M140 S%s"), buffer);
      enqueue_and_echo_command(cmd);
      Pcontrol.show();
      CamaObj.setText(buffer);
  }
  */
  //slider fan
  //Para ahorrar espacio se uso un Gcode mastro que ejecuta funcines
  void masterGcodePopCallback (/*void *ptr*/) {
    memset(buffer, 0, sizeof(buffer));
    SetVar.getText(buffer, sizeof(buffer));
    enqueue_and_echo_command(buffer);
    //_enqueuecommand(buffer);
  }
  void pararM600 (/*void *ptr*/) {
    //enqueue_and_echo_commands_P(PSTR("M600"));
      card.sdprinting = false;
      card.closeFile();
      clear_command_queue();
      quickstop_stepper();
      print_job_counter.stop();
      autotempShutdown();
      wait_for_heatup = false;
  }
  void sdrestPopCallback(){
    memset(buffer, 0, sizeof(buffer));
    ConGcode.getText(buffer, sizeof(buffer));
    enqueue_and_echo_command(buffer);
    Pcontrol.show();
    if (card.cardOK && SDstatus != 1) {
      SDstatus = 1;
      SD.setValue(1);
      SdBot.setShow();
      NPlay.setHide();
      SdRest.setHide();
      //NStop.setHide();
    }
  }
  void sdStatusTres(){
    SDstatus = 3;
    SD.setValue(3);
    enqueue_and_echo_commands_P(PSTR("M117 listo"));
  }
  void sdStatusDos(){
    SDstatus = 2;
    SD.setValue(2);
    enqueue_and_echo_commands_P(PSTR("M117 listo"));
  }
  //Expulsar tarjeta sd
  void sdExtiPopCallback(){
    enqueue_and_echo_commands_P(PSTR("M22"));
    Pcontrol.show();
    SdRest.setShow();
    if (!card.cardOK && SDstatus != 0){
      SDstatus = 0;
      SD.setValue(0);
      SdBot.setHide();
      NPlay.setHide();
      SdRest.setShow();
    }
  }
  //on fan prende el ventilador en 255
  /*
  void SetOnFanPopCallback (void *ptr) {
    fanSpeed = 255;
    Pcontrol.show();
    FanSp.setValue(fanSpeed);
    MostrarFan.setText("100");
  }
  */
  //Velocidad
  /*
  void setVeloPopCallback (void *ptr) {
    memset(buffer, 0, sizeof(buffer));
    VeloVar.getText(buffer, sizeof(buffer));
      char cmd[30];
      char* c;
      sprintf_P(cmd, PSTR("M220 S%s"), buffer);
      enqueue_and_echo_command(cmd);
      Pcontrol.show();
      CamaObj.setText(buffer);
  }
  */
  //hola hola
  void UploadNewFirmware() {
    if(IS_SD_INSERTED || card.cardOK) {
      Firmware.startUpload();
      nexSerial.end();
      lcd_init();
    }
  }
  void cambioOnOff() {
    if(CambioOff){
      CambioOff = false;
    }else{
      CambioOff = true;
    }
  }
  void salirPausaOnOff() {
    if(SalirPausaC){
      SalirPausaC = false;
    }else{
      SalirPausaC= true;
    }
  }
  bool salirPausa(){
    return SalirPausaC;
  }
  bool cargarOn(){
     return CambioOff;
  }
  //
  void BotCambiarFilamento(void){
    enqueue_and_echo_commands_P(PSTR("M600"));
  }
  //PausAFila
  //BotReanudarFilamento
  /*
  void BotReanudarFilamento(void){
    PausAFila.setValue(3);
  }
  */
  //apaga el ventilador
  /*
  void SetOffFanPopCallback (void *ptr) {
    fanSpeed = 0;
    Pcontrol.show();
    FanSp.setValue(fanSpeed);
    MostrarFan.setText("0");
  }
  */
  //mover ejes
  //Encargado de mover los ejes y hacer home
  void BotHomePopCallback (void *ptr) {
    memset(buffer, 0, sizeof(buffer));
    ComandoM.getText(buffer, sizeof(buffer));
    enqueue_and_echo_commands_P(PSTR("G91"));
    enqueue_and_echo_command(buffer);
      enqueue_and_echo_commands_P(PSTR("G90"));
  }
  /*
  void BotFilamentPopCallback (void *ptr) {
    memset(buffer, 0, sizeof(buffer));
    VarGcode.getText(buffer, sizeof(buffer));
    enqueue_and_echo_commands_P(PSTR("G91"));
    enqueue_and_echo_command(buffer);
    enqueue_and_echo_commands_P(PSTR("G90"));
  }
  */
  //inicio para calcular el offset
  void offsetInicioPopCallback() {
    enqueue_and_echo_commands_P(PSTR("M666 P-10\nG28\nG1 Z10"));
  }
  void BotHomeMasPopCallback (void *ptr) {
    memset(buffer, 0, sizeof(buffer));
    GcodeC.getText(buffer, sizeof(buffer));
    enqueue_and_echo_commands_P(PSTR("G91"));
    enqueue_and_echo_command(buffer);
    enqueue_and_echo_commands_P(PSTR("G90"));
  }
  void BotCalibrarPopCallback (void *ptr) {
    uint32_t number = 0;
    CoordC.getValue(&number);
    if(number == 7){
    }
    if(number == 6){
      CoordC.setValue(7);
      enqueue_and_echo_commands_P(PSTR("G1 Z10 F1000"));
      enqueue_and_echo_commands_P(PSTR("G28"));
    }
    if(number == 5){
      CoordC.setValue(6);
      enqueue_and_echo_commands_P(PSTR("G1 Z10 F1000"));
      enqueue_and_echo_commands_P(PSTR("G1 X100 Y100 F4800"));
      enqueue_and_echo_commands_P(PSTR("G28 Z"));
    }
    if(number == 4){
      CoordC.setValue(5);
      enqueue_and_echo_commands_P(PSTR("G1 Z10 F1000"));
      enqueue_and_echo_commands_P(PSTR("G1 X50 Y150 F4800"));
      enqueue_and_echo_commands_P(PSTR("G28 Z"));
    }
    if(number == 3){
      CoordC.setValue(4);
      enqueue_and_echo_commands_P(PSTR("G1 Z10 F1000"));
      enqueue_and_echo_commands_P(PSTR("G1 X150 Y150 F4800"));
      enqueue_and_echo_commands_P(PSTR("G28 Z"));
    }
    if(number == 2){
      CoordC.setValue(3);
      enqueue_and_echo_commands_P(PSTR("G1 Z10 F1000"));
      enqueue_and_echo_commands_P(PSTR("G1 X150 Y40 F4800"));
      enqueue_and_echo_commands_P(PSTR("G28 Z"));
    }
    if(number == 1){
      CoordC.setValue(2);
      enqueue_and_echo_commands_P(PSTR("G1 Z10 F1000"));
      enqueue_and_echo_commands_P(PSTR("G1 X50 Y40 F4800"));
      enqueue_and_echo_commands_P(PSTR("G28 Z"));

    }
    if(number == 0){
      CoordC.setValue(1);
      enqueue_and_echo_commands_P(PSTR("G1 Z10 F1000"));
      enqueue_and_echo_commands_P(PSTR("G28"));
    }
    //memset(buffer, 0, sizeof(buffer));

    //enqueue_and_echo_commands_P(PSTR("G91"));
    //enqueue_and_echo_command(buffer);
    //enqueue_and_echo_commands_P(PSTR("G90"));
  }

  //Temperatura del teclado setTecHotPopCallback
  /*
  static void setTecHotPopCallback(const char* filename) {
    memset(buffer, 0, sizeof(buffer));
    TemObjTec.getText(buffer, sizeof(buffer));
      char cmd[30];
      char* c;
      sprintf_P(cmd, PSTR("M140 S%s"), buffer);
      enqueue_and_echo_command(cmd);
      Pcontrol.show();
      HotendObj.setText(buffer);
  }
  */
  /*
  void settempPopCallback(void *ptr) {
    uint16_t number;

    memset(buffer, 0, sizeof(buffer));
    set0.getText(buffer, sizeof(buffer));

    number = atoi(buffer);

    if (ptr == &tup) number += 1;
    if (ptr == &tdown) number -= 1;

    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);

    set0.setText(buffer);
  }
  */
  /*
  void sethotPopCallback(void *ptr) {
    memset(buffer, 0, sizeof(buffer));
    set1.getText(buffer, sizeof(buffer));
    enqueue_and_echo_commands_P(buffer);
    setpageInfo();
  }
  */
  /*
  void set00PopCallback(void *ptr) {
    memset(buffer, 0, sizeof(buffer));
    Tgcode.getText(buffer, sizeof(buffer));
    enqueue_and_echo_commands_P(buffer);
    Pmenu.show();
  }
  */
  /*
  void setfanPopCallback(void *ptr) {
    if (fanSpeed) {
      fanSpeed = 0;
      fantimer.disable();
    }
    else {
      fanSpeed = 255;
      fantimer.enable();
    }
  }
  */
  //Punto de interes
  /*
  void actualizar(void *ptr){
    mostrarTempHot(0, degTargetHotend(0));
    mostrarTempHot(1, degHotend(0));
    mostrarTempHot(2, degTargetBed());
    mostrarTempHot(3, degBed());
  }
  */
  /*void setmovePopCallback(void *ptr) {
    memset(buffer, 0, sizeof(buffer));
    movecmd.getText(buffer, sizeof(buffer));
    enqueue_and_echo_commands_P(PSTR("G91"));
    enqueue_and_echo_commands_P(buffer);
    enqueue_and_echo_commands_P(PSTR("G90"));
  }
  */

  #if ENABLED(SDSUPPORT)
  //pausa y continuar
    void PlayPausePopCallback(void *ptr) {
      if (card.cardOK && card.isFileOpen()) {
        if (IS_SD_PRINTING)
          card.pausePrint();
        else
          card.startPrint();
      }
    }
    //para la impresion(no sibe mas

    void StopPopCallback(void *ptr) {
      if (card.cardOK && card.isFileOpen()) {
        quickstop_stepper();
        card.stopPrint();
        autotempShutdown();
        lcd_setstatus(MSG_PRINT_ABORTED, true);
      }
      else{
          enqueue_and_echo_commands_P(PSTR("M112"));
      }
    }
  #endif

  void lcd_init() {
    HAL::delayMilliseconds(2000);

    for (uint8_t i = 0; i < 10; i++) {
      NextionON = nexInit();
      if (NextionON) break;
      HAL::delayMilliseconds(1000);
    }

    if (!NextionON) {
      SERIAL_EM("Pantalla Tactil Kuttercraft No Conectada");
    }
    else {
      SERIAL_EM("Pantalla Tactil Kuttercraft Conectada");

      Pstart.attachPop(ExitPopCallback);
      //Exit1.attachPop(ExitPopCallback);
      //Exit3.attachPop(ExitPopCallback);
      /*
      #if ENABLED(NEXTION_GFX)
        gfx.color_set(VC_AXIS + X_AXIS, 63488);
        gfx.color_set(VC_AXIS + Y_AXIS, 2016);
        gfx.color_set(VC_AXIS + Z_AXIS, 31);
        gfx.color_set(VC_MOVE, 2047);
        gfx.color_set(VC_TOOL, 65535);
      #endif
      */

      #if ENABLED(SDSUPPORT)
        SdRest.attachPop(sdrestPopCallback, &SdRest);
        Expulsar.attachPop(sdExtiPopCallback, &Expulsar);
        SdBot.attachPop(setpageSDPopCallback);
        //MSD3.attachPop(setpageSDPopCallback);
        //MSD5.attachPop(setpageSDPopCallback);
        //MSD6.attachPop(setpageSDPopCallback);
        //MSD9.attachPop(setpageSDPopCallback);
          // sd continuar
        sdlist.attachPop(sdlistPopCallback);
        Mas.attachPop(sdlistPopCallback);
        Menos.attachPop(sdlistPopCallback);
        //Exit2.attachPop(ExitPopCallback);
        NPlay.attachPop(PlayPausePopCallback);

        Parar.attachPop(StopPopCallback);
      #endif

      /*
      #if ENABLED(RFID_MODULE)
        Rfid0.attachPop(rfidPopCallback,  &Rfid0);
        Rfid1.attachPop(rfidPopCallback,  &Rfid1);
        Rfid2.attachPop(rfidPopCallback,  &Rfid2);
        Rfid3.attachPop(rfidPopCallback,  &Rfid3);
        Rfid4.attachPop(rfidPopCallback,  &Rfid4);
        Rfid5.attachPop(rfidPopCallback,  &Rfid5);
      #endif
      */
      /*
      #if HAS_TEMP_0 //espera un toque
        //hot0.attachPop(hotPopCallback,      &hot0);
      #endif
      #if HAS_TEMP_1
        hot1.attachPop(hotPopCallback,      &hot1);
      #endif
      #if HAS_TEMP_2 || HAS_TEMP_BED
        hot2.attachPop(hotPopCallback,      &hot2);
      #endif
      */
      //Fanpic.attachPop(setfanPopCallback,   &Fanpic);
      //TemObjMod.attachPush(setHotenPopCallback,   &TemObjMod);
      //SetCama.attachPop(setCamaPopCallback,      &SetCama);
      //precalentamiento Hotend
      //BotPla.attachPop(setHotenPopCallback,     &BotPla);
      //BotAbs.attachPop(setHotenPopCallback,     &BotAbs);
      //BotFle.attachPop(setHotenPopCallback,     &BotFle);
      //BotNay.attachPop(setHotenPopCallback,     &BotNay);
      //precalentamiento cama
      //BotBedPla.attachPop(setCamaPopCallback,     &BotBedPla);
      //BotBedAbs.attachPop(setCamaPopCallback,     &BotBedAbs);
      //BotBedFle.attachPop(setCamaPopCallback,     &BotBedFle);
      //BotBedNay.attachPop(setCamaPopCallback,     &BotBedNay);
      //teclado
      //SetTecHot.attachPush(setTecHotPopCallback,   &SetTecHot);//setTecBedPopCallback SetTecBed
      //SetTecBed.attachPop(setTecBedPopCallback,    &SetTecBed);
      //ajustes de fan SetFan
      SetVar.attachPop(masterGcodePopCallback,          &SetVar);//ejecuta el mgcode
      //SetOnFan.attachPop(SetOnFanPopCallback,   &SetOnFan);//SetOffFanPopCallback
      //SetOffFan.attachPop(SetOffFanPopCallback, &SetOffFan);
      //ajuste velocidad
      //SetVelo.attachPop(setVeloPopCallback,        &SetVelo);
      //Mover eje
      //BotIzq.attachPop(BotHomePopCallback,     &BotIzq);
      BotHome.attachPop(BotHomePopCallback,        &BotHome);
      //BotDer.attachPop(BotHomePopCallback,     &BotDer);

      //pagina 11 filamento

      //temfil es en quivalente a Reanudar y a cargar esto ahorra espacio
      TemFil.attachPop(salirPausaOnOff, &TemFil);  //Cuando esta en la pausa de M600 Permite cargar filamento idefinida mente
      TemFil.attachPush(cambioOnOff,    &TemFil);  //sale de la pausa de un M600
      TemFil2.attachPop(pararM600, &TemFil2);
      //TemFil.attachPop(BotFilamentPopCallback,     &TemFil);
      //Cambiar.attachPop(BotCambiarFilamento,       &Cambiar);
      //Reanudar.attachPop(hola_hola,     &Reanudar);


      //pagina 12
      BotSetCal.attachPop(BotCalibrarPopCallback,  &BotSetCal); //BotHomeMasPopCallback
      MasHome.attachPop(BotHomeMasPopCallback,     &MasHome);//offsetTex
      offsetTex.attachPop(offsetInicioPopCallback,     &offsetTex);

      //offsetTex.attachPop(), &attachPop)
      //Boton.attachPop(setBotonPopCallback,   &Boton);
      //m11.attachPop(sethotPopCallback,      &m11);
      //tup.attachPop(settempPopCallback,     &tup);
      //tdown.attachPop(settempPopCallback,   &tdown);
      //XYHome.attachPop(setmovePopCallback);
      //XYUp.attachPop(setmovePopCallback);
      //XYRight.attachPop(setmovePopCallback);
      //XYDown.attachPop(setmovePopCallback);
      //XYLeft.attachPop(setmovePopCallback);
      //ZHome.attachPop(setmovePopCallback);
      //ZUp.attachPop(setmovePopCallback);
      //ZDown.attachPop(setmovePopCallback);
      //SpeedOk.attachPop(ExitPopCallback);
      //Benter.attachPop(setgcodePopCallback);

      startimer.enable();
    }
  }
  //punto de interes N:3
  //tiempo a terminar
  static void tiempoA(const char* message) {
    TimerEnd.setText(message);
  }
  static void mostrarTempHot(int h, int T1) {
    //creo un char para guardar un valor
    char valuetemp[10] = {0};
    //llamo a biffer y lo limpio
    memset(buffer, 0, sizeof(buffer));
    itoa(T1, valuetemp, 10);//¿lo tranforma, t1 = temperatura, valuetemp=val, 10=¿?. ?
    strcat(buffer, valuetemp);//¿Escribe en buffer lo que hay en valuetemp?
    //Lista de objetas a la que puedo preguntar la temperatura
    hotend_list[h]->setText(buffer);
  }
  static void coordtoLCD() {
    char* valuetemp;
    char* valorOffset;
    float offsetPrint;

    memset(buffer, 0, sizeof(buffer));
    strcat(buffer, (axis_known_position[X_AXIS] ? "X" : "? 000"));
    if (axis_homed[X_AXIS]) {
      valuetemp = ftostr4sign(current_position[X_AXIS]);
      strcat(buffer, valuetemp);
    }

    strcat(buffer, (axis_known_position[Y_AXIS] ? " Y" : " ? 000"));
    if (axis_homed[Y_AXIS]) {
      valuetemp = ftostr32sp(current_position[Y_AXIS] + 0.00001);
      strcat(buffer, valuetemp);
    }

    strcat(buffer, (axis_known_position[Z_AXIS] ? " Z " : " ? 000.00"));
    if (axis_homed[Z_AXIS]) {
      valuetemp = ftostr32sp(current_position[Z_AXIS] + 0.00001);
      strcat(buffer, valuetemp);
    }
    if (axis_homed[Z_AXIS]) {
      offsetPrint = current_position[Z_AXIS];
      offsetPrint = 10 - offsetPrint;
      valorOffset = ftostr32sp(offsetPrint + 0.00001);
      String stringOne = valorOffset;//convierto valorOffset en un string
      stringOne.trim(); //elimino los espacios en blanco
      stringOne.toCharArray(valorOffset, 20);//comvierto a stringOne en un char y lo guardo en valorOffset
      //String leetString = "as  ssasas";
      //leetString = leetString.trim();
      /*
      for(int i=0; i<strlen(asssasas); ++i){
        if (asssasas[i]==32) {
          espacio=i;
          aux=i;
          while (asssasas[aux]==32 && aux<strlen(asssasas)-1) aux;
          asssasas[espacio]=asssasas[aux];
          asssasas[aux]=32;
        }
      }
      */
    }
    LedCoord1.setText(buffer);
    CoordText.setText(buffer);
    offsetTex.setText(valorOffset);
    //CoordC.setText(buffer);
    //LedCoord6.setText(buffer);
  }
  //punto de interes n:4 se repite cada tantos ms
  void lcd_update() {

    if (!NextionON) return;

    nexLoop(nex_listen_list);

    millis_t ms = millis();

    if (ms > next_lcd_update_ms) {

      sendCurrentPageId(&NextionPage);

      if (NextionPage == 1) {
        if (fanSpeed > 0){
           fantimer.enable();
        }
        else{
           fantimer.disable();
           FanFoto.setPic(71);
        }
        //uint32_t temp_feedrate = 0;
        //VSpeed.getValue(&temp_feedrate);
        FanSp.setValue(fanSpeed);
        //punto de interes
        mostrarTempHot(0, degTargetHotend(0));
        mostrarTempHot(1, degHotend(0));
        mostrarTempHot(2, degTargetBed());
        mostrarTempHot(3, degBed());
        mostrarTempHot(4, (fanSpeed*100)/255);
        mostrarTempHot(5, feedrate_percentage);//VeloTxt

        /*
        #if HAS(TEMP_0) // se ejecuta al tocar
          temptoLCD(0, degHotend(0), degTargetHotend(0));
        #endif
        #if HAS(TEMP_1)
          temptoLCD(1, degHotend(1), degTargetHotend(1));
        #endif
        #if HAS(TEMP_2)
          temptoLCD(2, degHotend(2), degTargetHotend(2));
        #elif HAS(TEMP_BED)
          temptoLCD(2, degBed(), degTargetBed());
        #endif
        */
        coordtoLCD();
        //si esta activa la sd
        #if ENABLED(SDSUPPORT)
          //si esta imprimiendo
          if (card.isFileOpen()) {
            //si el status es distinto de 2
            if (SDstatus != 2 || SDstatus != 3) {
              SDstatus = 2;
              SD.setValue(2);
              NPlay.setShow();
              SdRest.setHide();
              //NStop.setShow();
            }
            if(IS_SD_PRINTING) {
              // Progress bar solid part
              //mientras imprime se muestra la pausa
              sdbar.setValue(card.percentDone());
              NPlay.setPic(107);

              // Estimate End Time
              //calcula un tiempo
              uint16_t time = print_job_counter.duration() / 60;
              uint16_t end_time = (time * (100 - card.percentDone())) / card.percentDone();
              if (end_time > (60 * 23)) {
                //tiempoA("Estimado: --h --m");
              }
              else if (end_time >= 0) {
                char temp[30];
                sprintf_P(temp, PSTR("Estimado: %i h %i m"), end_time / 60, end_time%60);
                tiempoA(temp);
              }
            }
            else {
              //si esta en pausa la imagen cambia por un reanudar
              NPlay.setPic(106);
            }
          }
          //si hay tarjeta y es distinto a 1
          else if (card.cardOK && SDstatus != 1 && SDstatus != 3) {
            SDstatus = 1;
            SD.setValue(1);
            SdBot.setShow();
            NPlay.setHide();
            SdRest.setHide();
            //NStop.setHide();
          }
          // si no hay tarjeta y el estatus es distinto de 0
          else if (!card.cardOK && SDstatus != 1 && SDstatus != 3) {
            SDstatus = 0;
            SD.setValue(0);
            SdBot.setHide();
            NPlay.setHide();
            SdRest.setShow();
            //NStop.setHide();
          }
        #endif

      }
      else if (NextionPage == 9) {
        coordtoLCD();
      }
      else if (NextionPage == 11) {
        mostrarTempHot(6, degHotend(0));
        mostrarTempHot(7, degTargetHotend(0));//temp log termal
      }
      else if (NextionPage == 12) {
        coordtoLCD();
      }
      else if (NextionPage == 14) {
        coordtoLCD();
      }
      next_lcd_update_ms = ms + LCD_UPDATE_INTERVAL;
    }

  }

  void lcd_setstatus(const char* message, bool persist) {
    if (lcd_status_message_level > 0 || !NextionON) return;
    strncpy(lcd_status_message, message, 30);
    InfoSet.setText(lcd_status_message);
  }

  void lcd_setstatuspgm(const char* message, uint8_t level) {
    if (level >= lcd_status_message_level && NextionON) {
      strncpy_P(lcd_status_message, message, 30);
      lcd_status_message_level = level;
      InfoSet.setText(lcd_status_message);
    }
  }

  void lcd_setalertstatuspgm(const char* message) {
    lcd_setstatuspgm(message, 1);
  }

  void lcd_reset_alert_level() { lcd_status_message_level = 0; }
  /*
  #if ENABLED(NEXTION_GFX)
    void gfx_clear(float x, float y, float z) {
      if ((NextionPage == 1) && (print_job_counter.isRunning() || IS_SD_PRINTING))
        gfx.clear(x, y, z);
    }
    void gfx_cursor_to(float x, float y, float z) {
      if ((NextionPage == 1) && (print_job_counter.isRunning() || IS_SD_PRINTING))
        gfx.cursor_to(x, y, z);
    }
    void gfx_line_to(float x, float y, float z){
      if ((NextionPage == 1) && (print_job_counter.isRunning() || IS_SD_PRINTING))
        gfx.line_to(VC_TOOL, x, y, z);
    }
    */


  /*********************************/
  /** Number to string conversion **/
  /*********************************/

  char conv[8];

  // Convert float to rj string with _123, -123, _-12, or __-1 format
  char *ftostr4sign(const float& x) { return itostr4sign((int)x); }

  // Convert float to space-padded string with -_23.4_ format
  char *ftostr32sp(const float &x) {
    long xx = abs(x * 100);
    uint8_t dig;

    if (x < 0) { // negative val = -_0
      conv[0] = '-';
      dig = (xx / 1000) % 10;
      conv[1] = dig ? '0' + dig : ' ';
    }
    else { // positive val = __0
      dig = (xx / 10000) % 10;
      if (dig) {
        conv[0] = '0' + dig;
        conv[1] = '0' + (xx / 1000) % 10;
      }
      else {
        conv[0] = ' ';
        dig = (xx / 1000) % 10;
        conv[1] = dig ? '0' + dig : ' ';
      }
    }

    conv[2] = '0' + (xx / 100) % 10; // lsd always

    dig = xx % 10;
    if (dig) { // 2 decimal places
      conv[5] = '0' + dig;
      conv[4] = '0' + (xx / 10) % 10;
      conv[3] = '.';
    }
    else { // 1 or 0 decimal place
      dig = (xx / 10) % 10;
      if (dig) {
        conv[4] = '0' + dig;
        conv[3] = '.';
      }
      else {
        conv[3] = conv[4] = ' ';
      }
      conv[5] = ' ';
    }
    conv[6] = '\0';
    return conv;
  }

  // Convert int to rj string with _123, -123, _-12, or __-1 format
  char* itostr4sign(const int& x) {
    int xx = abs(x);
    int sign = 0;
    if (xx >= 100) {
      conv[1] = (xx / 100) % 10 + '0';
      conv[2] = (xx / 10) % 10 + '0';
    }
    else if (xx >= 10) {
      conv[0] = ' ';
      sign = 1;
      conv[2] = (xx / 10) % 10 + '0';
    }
    else {
      conv[0] = ' ';
      conv[1] = ' ';
      sign = 2;
    }
    conv[sign] = x < 0 ? '-' : ' ';
    conv[3] = xx % 10 + '0';
    conv[4] = 0;
    return conv;
  }
#endif
