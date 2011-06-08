Name:           ffDiaporama
Version:        0.99.2
Release:        1%{?dist}
Summary:        Movie creator from photos and video clips
License:        GPLv2
URL:            http://ffdiaporama.tuxfamily.org
Group:		Applications/Multimedia
Icon:		logo.png
Source0:        ffDiaporama-0.99.2.tar.gz
BuildRoot: 	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:  qt-devel, ffmpeg-devel, SDL-devel, SDL_mixer-devel, desktop-file-utils
Requires:       ffmpeg

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
%doc
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
