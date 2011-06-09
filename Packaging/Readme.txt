Packaging FEDORA :
------------------
Préparation du PC de packaging :

su
yum groupinstall "Development Tools"
yum install qt-devel ffmpeg-devel SDL-devel SDL_mixer-devel rpmdevtools yum-utils
yum install qt ffmpeg SDL SDL_mixer exiv2

