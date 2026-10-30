# patches/patch-include_cereal_types_tuple_hpp: drop the redundant
# "template" keyword in ::template apply(), which newer clang rejects.

COMMENT =	header-only C++11 serialization library

V =		1.3.2
GH_ACCOUNT =	USCiLab
GH_PROJECT =	cereal
GH_TAGNAME =	v${V}

CATEGORIES =	devel

HOMEPAGE =	https://uscilab.github.io/cereal/
MAINTAINER =	Frank Baster <frank@baster.nz>

# BSD-3-Clause
PERMIT_PACKAGE =	Yes

# headers only
PKG_ARCH =		*

COMPILER =		base-clang
MODULES =		devel/cmake

CONFIGURE_ARGS +=	-DJUST_INSTALL_CEREAL=ON \
			-DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
			-DCMAKE_INSTALL_PREFIX=${PREFIX}

do-test:
	cd ${WRKBUILD} && \
	${CXX} -std=c++17 -O1 \
	    -I${WRKINST}${LOCALBASE}/include \
	    ${FILESDIR}/cereal_test.cpp -o cereal_test && \
	./cereal_test

.include <bsd.port.mk>
