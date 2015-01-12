Name:       ico-vic-amb-plugin
Summary:    Automotive Message Broker is a vehicle network abstraction system
Version:    0.9.5
Release:    0
Group:      Automotive/ICO Homescreen
License:    LGPL-2.1
Source0:    %{name}-%{version}.tar.bz2
Source1001: ico-vic-amb-plugin.manifest

Requires(post):   /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Requires:       ico-uxf-utilities
BuildRequires:  cmake
BuildRequires:  boost-devel
BuildRequires:  pkgconfig(json)
BuildRequires:  automotive-message-broker-devel >= 0.10.0
BuildRequires:  ico-uxf-utilities-devel >= 0.9.04
BuildRequires:  ico-uxf-utilities >= 0.9.04
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  sed

%description 
Automotive Message Broker is a vehicle network abstraction system
Collection of plugins for automotive-message-broker

%prep
%setup -q -n %{name}-%{version}
cp %{SOURCE1001} .
sed -i 's#LIBDIR#%{_libdir}/#' packaging.in/config.tizen-ico

%build
%cmake
%__make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%make_install
mkdir -p %{buildroot}%{_sysconfdir}/ambd/examples/
cp src/AMBformat.conf %{buildroot}/%{_sysconfdir}/ambd/
cp packaging.in/config.tizen-ico %{buildroot}%{_sysconfdir}/ambd/examples/config.tizen-ico
mkdir -p %{buildroot}%{_bindir}
cp tool/ico_set_vehicleinfo %{buildroot}%{_bindir}/ico_set_vehicleinfo

%post
/sbin/ldconfig

# When installing ($1 == 1)
if [ $1 -eq 1 ] ; then
  # Back up the original configuration and create a symlink
  # to ours if the configuration file exists.
  if [ -f "%{_sysconfdir}/ambd/config.tizen" ]; then
    echo "moving original AMB config.tizen -> config.tizen.orig"
    mv "%{_sysconfdir}/ambd/config.tizen" "%{_sysconfdir}/ambd/config.tizen.orig"
    echo "creating symlink AMB config.tizen -> examples/config.tizen-ico"
    ln -sf "%{_sysconfdir}/ambd/examples/config.tizen-ico" "%{_sysconfdir}/ambd/config.tizen"
  fi
fi

/bin/systemctl restart ambd

%postun
/sbin/ldconfig

# When uninstalling ($1 == 0)
if [ $1 -eq 0 ] ; then
  # Return the original file if a symlink exists and
  # we have the backup around
  if [ -L "%{_sysconfdir}/ambd/config.tizen" ]; then
    if [ -f "%{_sysconfdir}/ambd/config.tizen.orig" ]; then
      echo "restoring AMB config file config.tizen.orig as config.tizen"
      mv "%{_sysconfdir}/ambd/config.tizen.orig" "%{_sysconfdir}/ambd/config.tizen"
    fi
  fi
fi

/bin/systemctl restart ambd

%files
%defattr(-,root,root,-)
%manifest %{name}.manifest
%{_libdir}/automotive-message-broker/*.so
%config %{_sysconfdir}/ambd/AMBformat.conf
%config %{_sysconfdir}/ambd/examples/config.tizen-ico
%{_datadir}/doc/automotive-message-broker/%{name}/README
%{_bindir}/ico_set_vehicleinfo
