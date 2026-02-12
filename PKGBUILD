# Maintainer: Your Name <youremail@domain.com>
pkgname=todo-bbs
pkgver=1.0.0
pkgrel=1
pkgdesc="A Retro-Styled Terminal TODO Manager"
arch=('x86_64' 'aarch64') # i686 is rare these days, but keep if you prefer
url="https://github.com/Codiak540/TODO-Manager"
license=('GPL3') # Update this to match your actual license version
depends=('gcc-libs')
makedepends=('cmake')
source=("$pkgname-$pkgver.tar.gz::$url/archive/V$pkgver.tar.gz")
# Run 'updpkgsums' to replace the 'SKIP' with a real hash
sha256sums=('8cc809b9fe2424449e157545ea5958798b5cadf77f6953cbe9e9f3a806f3b943')

build() {
    # It's cleaner to create a build directory
    cmake -B build -S "TODO-Manager-$pkgver" \
        -DCMAKE_BUILD_TYPE=None \
        -DCMAKE_INSTALL_PREFIX=/usr

    cmake --build build
}

package() {
    DESTDIR="$pkgdir" cmake --install build

    # Install the license file (mandatory if it's not a common system license)
    install -Dm644 "TODO-Manager-$pkgver/LICENSE" "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
