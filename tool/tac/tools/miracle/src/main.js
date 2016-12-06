
window.onload = function () {
    z80_init();
    miracle_init();
    miracle_reset();
    loadRom(getRom());
    start();
};

