#include <mrubyc.h>

#include "../include/midi.h"
#include "../include/usb_descriptors.h"
#include "picoruby-prk-midi/include/prk-midi.h"

uint8_t const cable_num = 0; // MIDI jack associated with USB endpoint
uint8_t packet[4];

void Midi_init(void)
{
  while(tud_midi_available()) tud_midi_packet_read(packet);
}

void Midi_stream_write(uint8_t* packet, uint8_t len)
{
  tud_midi_stream_write(cable_num, packet, len);
}