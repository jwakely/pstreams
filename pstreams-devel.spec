Name:           pstreams-devel
Version:        1.0.3
Release:        6%{?dist}
Summary:        POSIX Process Control in C++

License:        Boost
URL:            http://pstreams.sourceforge.net/
Source0:        http://downloads.sourceforge.net/pstreams/pstreams-%{version}.tar.gz

BuildRequires:  make
BuildRequires:  gcc-c++
BuildRequires:  doxygen
BuildRequires:  perl
BuildRequires:  gawk
BuildRequires:  hostname
BuildArch:      noarch

%description
PStreams classes are like C++ wrappers for the POSIX.2 functions
popen(3) and pclose(3), using C++ iostreams instead of C's stdio
library.

%package -n pstreams-doc
Summary: Documentation for pstreams

%description -n pstreams-doc
API documentation for the pstreams-devel package, in HTML format.

%prep
%setup -q -n pstreams-%{version}

%build
make %{?_smp_mflags} docs

%check
make %{?_smp_mflags} EXTRA_CXXFLAGS="$CXXFLAGS" check

%install
make install  DESTDIR=$RPM_BUILD_ROOT includedir=%{_includedir}

%files
%license LICENSE_1_0.txt
%{_includedir}/pstreams

%files -n pstreams-doc
%doc doc/html README AUTHORS ChangeLog

%changelog
* Thu Sep 15 2022 Jonathan Wakely <jwakely@redhat.com> - 1.0.3-6
- Fix build by passing build flags in the relevant make variable
- Add pstreams-doc subpackage and separate check phase of build
- Add BuildRequires:hostname to fix tests

* Wed Jun 10 2020 Jonathan Wakely <jwakely@redhat.com> - 1.0.3-1
- Update to version 1.0.3, change license info
- Add BuildRequires: for gcc-c++ and fix BuildRequires: for awk

* Mon Mar 09 2020 Jonathan Wakely <jwakely@redhat.com> - 0.8.1-13
- Add BuildRequires: for perl and awk

* Thu Feb 15 2018 Jonathan Wakely <jwakely@redhat.com> - 0.8.1-8
- Remove unnecessary Group tag and buildroot cleanup

* Tue Jan 26 2016 Jonathan Wakely <jwakely@redhat.com> - 0.8.1-3
- Sync spec file with upstream.

* Tue Jan 05 2016 Jonathan Wakely <jwakely@redhat.com> - 0.8.1-3
- Replace packagename macro and remove BuildRoot tag.

* Sat Aug 16 2014 Pavel Alexeev <Pahan@Hubbitus.info> - 0.8.1-1
- Update to 0.8.1 (bz#1111872)

* Wed May 12 2010 Jonathan Wakely <pstreams@kayari.org> - 0.7.0-1
- Add spec file to upstream repo and update.

* Fri Nov 07 2008 Rakesh Pandit <rakesh@fedoraproject.org> 0.6.0-6
- timestamp patch (Till Mass)

* Fri Nov 07 2008 Rakesh Pandit <rakesh@fedoraproject.org> 0.6.0-5
- saving timestamp using "install -p"

* Fri Nov 07 2008 Rakesh Pandit <rakesh@fedoraproject.org> 0.6.0-4
- included docs, license and other missing files.

* Fri Nov 07 2008 Rakesh Pandit <rakesh@fedoraproject.org> 0.6.0-3
- consistent use of macros - replaced %%{buildroot} with $RPM_BUILD_ROOT

* Thu Nov 06 2008 Rakesh Pandit <rakesh@fedoraproject.org> 0.6.0-2
- Cleaned up buildrequire

* Tue Nov 04 2008 Rakesh Pandit <rakesh@fedoraproject.org> 0.6.0-1
- initial package
