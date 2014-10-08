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

%description 
Automotive Message Broker is a vehicle network abstraction system
Collection of plugins for automotive-message-broker

%prep
%setup -q -n %{name}-%{version}
cp %{SOURCE1001} .

%build
%cmake
%__make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%make_install
mkdir -p %{buildroot}/%{_sysconfdir}/ambd
cp src/AMBformat.conf %{buildroot}/%{_sysconfdir}/ambd/
mkdir -p %{buildroot}%{_bindir}
cp tool/ico_set_vehicleinfo %{buildroot}%{_bindir}/ico_set_vehicleinfo

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%manifest %{name}.manifest
%{_libdir}/automotive-message-broker/*.so
%config %{_sysconfdir}/ambd/AMBformat.conf
%{_datadir}/doc/automotive-message-broker/%{name}/README
%{_bindir}/ico_set_vehicleinfo
