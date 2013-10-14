Name:       ico-vic-amb-plugin
Summary:    Automotive Message Broker is a vehicle network abstraction system.
Version:    0.9.2
Release:    1.1
Group:      System Environment/Daemons
License:    LGPL v2.1
URL:        ""
Source0:    %{name}-%{version}.tar.bz2
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Requires:   libjson
Requires:  ico-uxf-utilities
BuildRequires:  cmake
BuildRequires:  boost-devel
BuildRequires:  libjson-devel
BuildRequires:  automotive-message-broker-devel >= 0.10.0
BuildRequires:  ico-uxf-utilities-devel
BuildRequires:  ico-uxf-utilities

%description 
Collection of plugins for automotive-message-broker

%prep
%setup -q -n %{name}-%{version}

%build
%cmake

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install
mkdir -p %{buildroot}/%{_sysconfdir}/ambd
mkdir -p %{buildroot}/usr/bin
cp src/AMBformat.conf %{buildroot}/%{_sysconfdir}/ambd/
cp tool/ico_set_vehicleinfo %{buildroot}/usr/bin/ico_set_vehicleinfo

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/automotive-message-broker/*.so
%{_sysconfdir}/ambd/AMBformat.conf
/usr/share/doc/automotive-message-broker/%{name}/README
/usr/bin/ico_set_vehicleinfo
