Name:           ffDiaporama
Version:        0.99.2
Release:        1%{?dist}
Summary:        FFDIAPORAMA - Movie creator
Summary(fr):    FFDIAPORAMA - Créateur de film
License:        GPLv2
URL:            http://ffdiaporama.tuxfamily.org
Group:		Applications/Multimedia
Icon:		logo.xpm
Source0:        ffDiaporama-0.99.2.tar.gz
BuildRoot: 	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:  qt-devel, ffmpeg-devel, SDL-devel, SDL_mixer-devel, desktop-file-utils
Requires:       qt, SDL, SDL_mixer, ffmpeg, exiv2

%description
ffDiaporama is an application of creation of videos sequences established by
titles, images, photos, movie clip and music.

These sequences are assembled in slide show by means of transitions of
sequence to produce complete videos

Main features:
* Refocused of images and photos and refocused and cutting of video clips
* Note (addition of text) for images, photos, sequences and animations
* Graphic filters on the images and the videos (passage in black and
  white, dust removal, equalization of colors, etc.)
* Creation of animation by zoom, rotation or Ken Burns Effect on part of
  images or photos
* Correction of the images and the videos during the animations
  (luminosity, contrast, gamma, colors, etc.)
* Transitions between sequence with definition of the transition type,
  sequence by sequence.
* Addition of a background sound (wav, mp3 or ogg) with customizable
  effects of volume, fade in/out and passage in pause, sequence by
  sequence.
* Generation of usable videos by most of the current videos equipments
  (DVD player/smartphone, multimedia box, hard drive, etc.) but also
  publishable on the main video sharing Web sites (YouTube, Dailymotion,
  etc.)

%description -l fr
ffDiaporama est une application de création de séquences vidéos constituées de
titres, d'images ou de photographies, de clips vidéo et de musiques.

Ces séquences sont assemblées en diaporama à l'aide de transitions de séquence
pour constituer des vidéos complètes.

Principales fonctionnalités :
* Recadrage des images, photos, titres et vidéos et découpage des clips vidéos
* Annotation (ajout de texte) des images, des photos, des vidéos et des
  animations
* Filtres graphiques sur les images et vidéos (passage en noir et blanc,
  dépoussiérage, égalisation des couleurs, etc...)
* Création d'animation par effet de zoom, de rotation, de déplacement sur des
  portions d'images (effet Ken Burns)
* Correction des images et des vidéos pendant les animations
  (correction luminosité, contraste, gamma, couleurs, etc...)
* Transitions entre séquences avec définition séquence par séquence.
* Ajout de musique pendant le diaporama avec effets de volume, de fondu et
  passage en pause paramétrables séquence par séquence.
* Génération de vidéos exploitables par la plupart des équipements vidéos
  (lecteurs de DVD de salon, disques durs multimédia, mobile/smartphone, etc.)
  mais également diffusables sur les principaux sites WEB de partage vidéo
  (Youtube, Dailymotion, etc.)

%prep
%setup -q

%build
qmake-qt4
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install INSTALL_ROOT=$RPM_BUILD_ROOT
desktop-file-install --vendor=""                                 \
       --dir=%{buildroot}%{_datadir}/applications/   \
       %{buildroot}%{_datadir}/ffDiaporama/ffDiaporama.desktop

%files
%doc %{_datadir}/ffDiaporama/licences.txt
%{_datadir}/
/usr/bin/ffDiaporama

%post
touch --no-create %{_datadir}/icons/hicolor
if [ -x %{_bindir}/gtk-update-icon-cache ]; then
  %{_bindir}/gtk-update-icon-cache -q %{_datadir}/icons/hicolor;
fi
update-mime-database %{_datadir}/mime &> /dev/null || :
update-desktop-database &> /dev/null || :

%postun
touch --no-create %{_datadir}/icons/hicolor
if [ -x %{_bindir}/gtk-update-icon-cache ]; then
  %{_bindir}/gtk-update-icon-cache -q %{_datadir}/icons/hicolor;
fi
update-mime-database %{_datadir}/mime &> /dev/null || :
update-desktop-database &> /dev/null || :

%changelog

* Wed Jun  8 2011 Dominique LEVRAY <levray.dominique@bbox.fr> %{version}-%{release}
- Initial release
