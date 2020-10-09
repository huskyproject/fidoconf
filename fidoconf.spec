%define ver_major 1
%define ver_minor 9
%define reldate 20191205
%define reltype C
# may be one of: C (current), R (release), S (stable)

# release number for Release: header
%define relnum 3

# on default static library is made but using 'rpmbuild --without static'
# produces a dynamic library
%bcond_without static

# if you use 'rpmbuild --with debug' then debug binary is produced
%bcond_with debug

# for generic build; will override for some distributions
%define vendor_prefix %nil
%define vendor_suffix %nil
%define pkg_group Libraries/FTN

# for CentOS, Fedora and RHEL
%if %_vendor == "redhat"
%define vendor_suffix %dist
%endif

# for ALT Linux
%if %_vendor == "alt"
%define vendor_prefix %_vendor
%define pkg_group Networking/FTN
%endif

%define main_name fidoconf
%if %{with static}
Name: %main_name-static
%else
Name: %main_name
%endif
Version: %ver_major.%ver_minor.%reldate%reltype
Release: %{vendor_prefix}%relnum%{vendor_suffix}
%if %_vendor != "redhat"
Group: %pkg_group
%endif
%if %{with static}
Summary: Common configuration static library for the Husky Project applications
%else
Summary: Common configuration dynamic library for the Husky Project applications
%endif
URL: https://github.com/huskyproject/%main_name/archive/v%ver_major.%ver_minor.%reldate.tar.gz
License: GPL
Source: %main_name-%ver_major.%ver_minor.%reldate.tar.gz
%if %{with static}
BuildRequires: huskylib-static huskylib-devel
BuildRequires: smapi-static smapi-devel
%else
BuildRequires: huskylib huskylib-devel
BuildRequires: smapi smapi-devel
Requires: huskylib smapi
%endif

%description
%summary


%package -n %main_name-devel
%if %_vendor != "redhat"
Group: %pkg_group
%endif
Summary: Development headers for %main_name
BuildArch: noarch
%description -n %main_name-devel
%summary


%if %{with static}
%define utilities %main_name-utils-static
%else
%define utilities %main_name-utils
%endif
%package -n %utilities
Summary: Optional utilities for %name
%if ! %{with static}
Requires: %name = %version-%release
%endif
Provides: %utilities = %version-%release
%description -n %utilities
%summary


%if %{with static}
%define parser tparser-static
%else
%define parser tparser
%endif
%package -n %parser
Summary: A utility for parsing and checking Husky Project configuration files
%if ! %{with static}
Requires: %name = %version-%release
%endif
%description -n %parser
%summary


%prep
%setup -q -n %main_name-%ver_major.%ver_minor.%reldate


%build
# parallel build appears to be broken in CentOS, Fedora and RHEL
%if %_vendor == "redhat"
    %if %{with static}
        %if %{with debug}
            make DEBUG:=1
        %else
            make
        %endif
    %else
        %if %{with debug}
            make DYNLIBS:=1 DEBUG:=1
        %else
            make DYNLIBS:=1
        %endif
    %endif
%else
    %if %{with static}
        %if %{with debug}
            %make DEBUG:=1
        %else
            %make
        %endif
    %else
        %if %{with debug}
            %make DYNLIBS:=1 DEBUG:=1
        %else
            %make DYNLIBS:=1
        %endif
    %endif
%endif
echo Install-name1:%_rpmdir/%_arch/%name-%version-%release.%_arch.rpm > /dev/null
echo Install-name2:%_rpmdir/noarch/%main_name-devel-%version-%release.noarch.rpm > /dev/null

# macro 'install' is omitted for debug build because it strips the library
%if ! %{with debug}
    %install
%endif
umask 022
%if %{with static}
    make DESTDIR=%buildroot install
%else
    make DESTDIR=%buildroot DYNLIBS=1 install
%endif
chmod -R a+rX,u+w,go-w %buildroot

%if %_vendor != "redhat"
%clean
rm -rf -- %buildroot
%endif

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%if %{with static}
    %_libdir/*.a
%else
    %exclude %_libdir/*.a
    %_libdir/*.so.*
    %_libdir/*.so
%endif

%files -n %main_name-devel
%dir %_includedir/%main_name
%_includedir/%main_name/*

%files -n %utilities
%_bindir/*
%exclude %_bindir/tparser

%files -n %parser
%_bindir/tparser
