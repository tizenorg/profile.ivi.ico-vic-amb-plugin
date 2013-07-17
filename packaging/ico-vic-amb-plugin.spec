Name:       ico-vic-amb-plugin
Summary:    Automotive Message Broker is a vehicle network abstraction system.
Version:    0.2.1
Release:    1
Group:      System Environment/Daemons
License:    LGPL v2.1
URL:        ""
Source0:    %{name}-%{version}.tar.bz2
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
#Requires:	json-glib
Requires:   libjson
Requires:  ico-uxf-utilities
BuildRequires:  cmake
BuildRequires:  boost-devel
#BuildRequires:  json-glib-devel
BuildRequires:  libjson-devel
BuildRequires:  automotive-message-broker-devel >= 0.6.9
BuildRequires:  ico-uxf-utilities-devel
#BuildRequires:  libwebsockets-devel >= 1.2

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
cp src/AMBformat.conf %{buildroot}/%{_sysconfdir}/ambd/

mkdir -p %{buildroot}/%{_sysconfdir}/ambd
cp src/AMBformat.conf %{buildroot}/%{_sysconfdir}/ambd/

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/automotive-message-broker/*.so
%{_sysconfdir}/ambd/AMBformat.conf
/usr/share/doc/automotive-message-broker/%{name}/README
