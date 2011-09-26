Name:           ffDiaporama
Version:        1.0.0
Release:        0%{?dist}
Distribution: 	openSUSE 11.4
Summary:        FFDIAPORAMA - Movie creator
Summary(fr):    FFDIAPORAMA - Créateur de film
Summary(it):    FFDIAPORAMA - Movie creator
Summary(es):    FFDIAPORAMA - Creador de películas
License:        GPLv2
URL:            http://ffdiaporama.tuxfamily.org
Group:		Applications/Multimedia
Icon:		logo.xpm
Source0:        ffDiaporama-1.0.0.tar.gz
BuildRoot: 	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:  libqt4-devel, libQtWebKit-devel, libffmpeg-devel, libSDL-devel, libSDL_mixer-devel, desktop-file-utils
Requires:       libqt4, libQtWebKit4, ffmpeg, libSDL-1_2-0, libSDL_mixer-1_2-0, exiv2

%description
ffDiaporama is an application for assembling photos and videos especially
for preserving holiday memories. It is inspired by presentation software
such as Impress or Powerpoint. It can read all image formats, a majority
of cameras and camcorder video formats, and the most commonly used music
formats. It includes 140 transitions and a device database for the
generation of videos with more than 50 models of video viewing devices
(including smartphones, tablets, and home cinema systems).

ffDiaporama can manage several hundreds of slides.
Every slide can contain several tens of shots and several tens of blocks.
Every shot contains its own block settings.
The application calculates all the shot transformations.

%description -l fr
ffDiaporama est un logiciel d'assemblage de photos et dee vidéos
particulièrement conçu pour préserver ses souvenirs de vacances.
Il s'inspire de logiciel de présentation tel que Impress ou Powerpoint.
Il peut lire tous les formats d'image, la majorité des vidéos produites par
les appareils photo et les caméscopes et les formats de musique les plus
courants.

Il inclut 140 transitions et une base de données de profils pour la
génération des vidéos pour plus de 50 modèles (incluant des smartphones,
des tablettes et des systèmes home cinema).

ffDiaporama peut gérer plusieurs centaines de diapositives. Chaque diapositive
peut contenir plusieurs dizaines de plans et plusieurs dizaines de blocs.
Chaque plan contient ses propres paramètres de bloc.
L'application calcule automatiquement toutes les transformations de plan.

%description -l it
ffDiaporama è un'applicazione per il montaggio di foto e video, specialmente
per conservare i ricordi delle vacanze. Si ispira da software per le
presentazioni come Powerpoint o Impress. Può leggere tutti i formati immagine,
una maggioranza di formati video di telecamere e videocamere, e formati
musicali più comunemente utilizzati. Comprende 140 transizioni e un database
dei dispositivi per la generazione di video con oltre 50 modelli di
dispositivi di visualizzazione video (Tra cui smartphone, tablet e sistemi
home cinema).

ffDiaporama è in grado di gestire diverse centinaia di diapositive.
Ogni diapositiva può contenere diverse decine di scatti e diverse decine di
blocchi. Ogni scatto contiene le propie impostazioni di blocco.
L'applicazione calcola tutte le trasformazioni di scatto.

%description -l es
ffDiaporama es una aplicación para manejar  fotos y vídeos,
especialmente para preservar los recuerdos de las vacaciones.
Está inspirado en programas de presentación como Impress o Powerpoint.
Puede trabajar con todos los formatos de imagen, La mayoría de los vídeos
producidos por las cámaras de fotos y las cámaras de vídeo, y los formatos de
música más utilizados.

Incluye 140 transiciones y una base de datos de perfiles para la
generación de los vídeos para más de 50 modelos (incluídos smartphones,
Tablets y sistemas home cinema).

ffDiaporama puede administrar varias centenas de diapositivas.
Cada diapositiva puede contener varias decenas de planos y varias decenas de
bloques. Cada plano contiene sus propios parámetros de bloque.
La aplicación calcula automáticamente todas las transformaciones del plano.

%prep
%setup -q

%build
qmake
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install INSTALL_ROOT=$RPM_BUILD_ROOT

%files
%doc %{_datadir}/ffDiaporama/licences.txt
%doc %{_datadir}/ffDiaporama/authors.txt
%{_datadir}/%{name}/
%{_bindir}/%{name}
%{_datadir}/applications/ffDiaporama.desktop
%{_datadir}/icons/gnome/32x32/mimetypes/application-ffDiaporama.png
%{_datadir}/mime/packages/ffDiaporama-mime.xml

%post
touch --no-create %{_datadir}/icons/hicolor
if [ -x %{_bindir}/gtk-update-icon-cache ]; then
  %{_bindir}/gtk-update-icon-cache -q %{_datadir}/icons/hicolor;
fi
update-mime-database %{_datadir}/mime &> /dev/null || :
gtk-update-icon-cache --force /usr/share/icons/gnome &> /dev/null || :
update-desktop-database &> /dev/null || :

%postun
touch --no-create %{_datadir}/icons/hicolor
if [ -x %{_bindir}/gtk-update-icon-cache ]; then
  %{_bindir}/gtk-update-icon-cache -q %{_datadir}/icons/hicolor;
fi
update-mime-database %{_datadir}/mime &> /dev/null || :
gtk-update-icon-cache --force /usr/share/icons/gnome &> /dev/null || :
update-desktop-database &> /dev/null || :

%changelog

* Mon Sep 26 2011 Dominique LEVRAY <levray.dominique@bbox.fr> 1.0.0-0%{?dist}
- Release 20110926
