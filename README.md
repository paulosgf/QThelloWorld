# lightdmTheme
QT 5 LightDM Theme

- Create these files

cat /usr/share/xgreeters/lightdmTheme.desktop
[Desktop Entry]
Name=lightdmTheme Greeter
Exec=/usr/local/bin/lightdmTheme
Type=Application

cat /usr/share/xsessions/lightdmTheme.desktop 
Name=lightdmTheme Greeter
Exec=/usr/local/bin/lightdmTheme
Type=Application

- Configure default theme

cat /etc/lightdm/lightdm.conf
[Seat:*]
greeter-session=lightdmTheme
user-session=mate
greeter-wrapper=/etc/lightdm/Xgreeter
session-wrapper=/etc/lightdm/Xsession
greeter-hide-users=false
greeter-allow-guest=false
logind-check-graphical=true
