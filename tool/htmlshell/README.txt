Este archivo contiene información util a tener en cuenta cuando se actualize la
version de chromium utilizada (actualmente se utiliza la version 47.0.2526.106).

* Los principales directorios de chromium en los que se deberian buscar cambios importantes que
  pueden afectar nuestra implementacion son:
    CHROMIUM_SRC/
        base/
        content/
        net/
        ui/
        webkit/
        third_party/WebKit/
        url/
        v8/
        media/

* Las clases ubicadas en el directorio $DEPOT/tool/htmlshell/htmlshell/src/impl/ui/aura/
  fueron copiadas desde $DEPOT/tool/htmlshell/ui/aura/test/ para no depender de la libreria
  aura_test_support, ya que solo son necesarias algunas clases.

* La implementacion de la content lib se encuentra en $DEPOT/tool/htmlshell/common/overlays/htmlshell/src/impl/
  siguiendo la misma estructura que content_shell, por lo cual, algun cambio significativo en
  las principales clases de content_shell casi seguro implicaran revisar nuestra implementacion.

-------------------------------------------------------------------------------------------------------

WIDEVINE:
Sacar widevine de una imagen de ChromeOS:

Buscar la version estable de chrome-os en https://cros-omahaproxy.appspot.com/
Ej: CHROMEOS_VERSION=7647.84.0

Bajar la imagen de recovery desde:
x86_64: # wget https://dl.google.com/dl/edgedl/chromeos/recovery/chromeos_${CHROMEOS_VERSION}_lulu_recovery_stable-channel_mp.bin.zip
arm_v7: # wget https://dl.google.com/dl/edgedl/chromeos/recovery/chromeos_${CHROMEOS_VERSION}_daisy-skate_recovery_stable-channel_skate-mp.bin.zip
# unzip chromeos_${CHROMEOS_VERSION}_[...].bin.zip
# sudo kpartx -av chromeos_${CHROMEOS_VERSION}_[...].bin
# mkdir img
# sudo mount -r /dev/mapper/loop0p3 img
# cp img/opt/google/chrome/libwidevine* <build-dir>/rootfs/profiles/system/plugins

Para aml o bien hay que armar una toolchain con la libc actualizada o copiar a mano las libs de chromeos:
# cp img/lib/ld-linux-armhf.so.3  lib/
# cp img/lib/libc.so.6            lib/
# cp img/lib/libdl.so.2           lib/
# cp img/lib/libpthread.so.0      lib/
# cp img/lib/libm.so.6            lib/
# cp img/usr/lib/libstdc\+\+.so.6 usr/lib/
# cp img/usr/lib/libgcc_s.so.1    usr/lib/

# ./run.sh https://netflix.com/ --plugins=profiles/system/plugins/widevine_plugin.json --allowed-plugins="Widevine Content Decryption Module" --extra-ua="Mozilla/5.0 (X11; CrOS armv7l ${CHROMEOS_VERSION})"
