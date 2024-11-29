# quectel-open-source :: GNSSTool

https://github.com/quectel-open-source/GNSSTool

## Build (WIP)

```bash
qmake -project
```

editer GNSSTool.pro resultant et ajouter
```
QT += gui widgets serialport network webenginewidgets
INCLUDEPATH += ./QGNSS_FlashTool_UI/addsrc/Control ./QGNSS_FlashTool_UI/src/Misc ./QGNSS_BROM/IO ./QGNSS_BROM ./QGNSS_FlashTool_UI/addsrc ./QGNSS_BROM/MTK ./QGNSS_BROM/Protocol ./QGNSS ./QGNSS_FlashTool_UI/addsrc/MTK/Widgets ./QGNSS_FlashTool_UI/addsrc/MTK/Widgets/Component ./QGNSS_FlashTool_UI/addsrc/CommonWidgets ./QGNSS_FlashTool_UI/addsrc/BaseWidgets ./QGNSS_FlashTool_UI/addsrc/MTK/DATACENTER ./QGNSS_FlashTool_UI/addsrc/MTK/MODEL ./QGNSS_FlashTool_UI/addsrc/Broadcom/Widgets ./QGNSS_FlashTool_UI/addsrc/Broadcom/Widgets/Component ./QGNSS_BROM/Broadcom ./QGNSS_FlashTool_UI/addsrc/Broadcom/MODEL ./QGNSS_FlashTool_UI/addsrc/ST/Widgets ./QGNSS_FlashTool_UI/addsrc/ST/MODEL ./QGNSS_BROM/ST ./QGNSS_FlashTool_UI/addsrc/ST/Widgets/Component ./QGNSS_FW_UPG/QGNSS_FW_UPG/st/teseo5 ./QGNSS_FW_UPG/QGNSS_FW_UPG/uart ./QGNSS_FW_UPG/QGNSS_FW_UPG ./QGNSS_FW_UPG ./QGNSS_FlashTool_UI/addsrc/Goke/Widgets ./QGNSS_FlashTool_UI/addsrc/Goke/Widgets/Component ./QGNSS_BROM/Goke ./QGNSS_FlashTool_UI/addsrc/Goke/MODEL ./QGNSS_FlashTool_UI/addsrc/Allystar/Widgets ./QGNSS_FlashTool_UI/addsrc/Allystar/MODEL ./QGNSS_BROM/Allystar ./QGNSS_FlashTool_UI/addsrc/Allystar/Widgets/Component ./QGNSS_FlashTool_UI/addsrc/Unicorecomm/Widgets ./QGNSS_FlashTool_UI/addsrc/Unicorecomm/MODEL ./QGNSS_BROM/Unicorecomm ./QGNSS_FlashTool_UI/addsrc/Unicorecomm/Widgets/Component
```

```bash
qmake GNSSTool.pro
make
```
