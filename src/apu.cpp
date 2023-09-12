/*
 * apu.cpp
 * 
 * The audio module here uses a modified version of NotSoFatso, a Winamp plugin
 * for sound generation
 * 
 * Original can be found here:
 * https://disch.zophar.net/notsofatso.php
*/

#include "apu.h"

#include <stdio.h>
#include <NSF_Core.h>
#include <NSF_File.h>

int16_t samples[(APU_SAMPLES_SIZE/sizeof(int16_t))+0x20];
const int16_t *apu_samples = samples;

CNSFCore *nsf_core;
uint8_t nsf_dummy_code[3] = {0x4c,0x00,0x80}; // jmp $8000

void apu_init(void) {
    CNSFFile nsf_dummy_file;
    // file.LoadFile("/home/tavispalmer/Downloads/smb.nsf", true, false);
    // file.LoadFile("/home/tavispalmer/Downloads/smb.nsf", true, false);
    memset(&nsf_dummy_file,0,sizeof(CNSFFile));
    nsf_dummy_file.nLoadAddress = 0x8000;
    nsf_dummy_file.nInitAddress = 0x8000;
    nsf_dummy_file.nPlayAddress = 0x8000;

    nsf_dummy_file.nNTSC_PlaySpeed = 16666;
    nsf_dummy_file.nTrackCount = 1;
    
    nsf_dummy_file.pDataBuffer = nsf_dummy_code;
    nsf_dummy_file.nDataBufferSize = sizeof(nsf_dummy_code);

    nsf_core = new CNSFCore;
    nsf_core->Initialize();
    nsf_core->SetPlaybackOptions(APU_SAMPLE_RATE, APU_CHANNEL_COUNT);
    nsf_core->LoadNSF(&nsf_dummy_file);
    nsf_core->SetTrack(0);
    nsf_core->SetChannelOptions(0, 1, 255, -45, 1);
    nsf_core->SetChannelOptions(1, 1, 255, 45, 1);
    nsf_core->SetChannelOptions(2, 1, 255, 0, 0);
    nsf_core->SetChannelOptions(3, 1, 255, 0, 0);

    nsf_dummy_file.pDataBuffer = NULL;
}

void apu_run(void) {
    nsf_core->GetSamples((BYTE*)samples, APU_SAMPLES_SIZE);
}

uint8_t apu_read_snd_chn(void) {
    return nsf_core->ReadMemory_pAPU(0x4015);
}

void apu_write_sq1_vol(uint8_t value) {
    nsf_core->WriteMemory_pAPU(0x4000, value);
}

void apu_write_sq1_sweep(uint8_t value) {
    nsf_core->WriteMemory_pAPU(0x4001, value);
}

void apu_write_sq1_lo(uint8_t value) {
    nsf_core->WriteMemory_pAPU(0x4002, value);
}

void apu_write_sq1_hi(uint8_t value) {
    nsf_core->WriteMemory_pAPU(0x4003, value);
}

void apu_write_sq2_vol(uint8_t value) {
    nsf_core->WriteMemory_pAPU(0x4004, value);
}

void apu_write_sq2_sweep(uint8_t value) {
    nsf_core->WriteMemory_pAPU(0x4005, value);
}

void apu_write_sq2_lo(uint8_t value) {
    nsf_core->WriteMemory_pAPU(0x4006, value);
}

void apu_write_sq2_hi(uint8_t value) {
    nsf_core->WriteMemory_pAPU(0x4007, value);
}

void apu_write_tri_linear(uint8_t value) {
    nsf_core->WriteMemory_pAPU(0x4008, value);
}

void apu_write_tri_lo(uint8_t value) {
    nsf_core->WriteMemory_pAPU(0x400a, value);
}

void apu_write_tri_hi(uint8_t value) {
    nsf_core->WriteMemory_pAPU(0x400b, value);
}

void apu_write_noise_vol(uint8_t value) {
    nsf_core->WriteMemory_pAPU(0x400c, value);
}

void apu_write_noise_lo(uint8_t value) {
    nsf_core->WriteMemory_pAPU(0x400e, value);
}

void apu_write_noise_hi(uint8_t value) {
    nsf_core->WriteMemory_pAPU(0x400f, value);
}

void apu_write_dmc_freq(uint8_t value) {

}

void apu_write_dmc_raw(uint8_t value) {
    nsf_core->WriteMemory_pAPU(0x4011, value);
}

void apu_write_dmc_start(uint8_t value) {

}

void apu_write_dmc_len(uint8_t value) {

}

void apu_write_snd_chn(uint8_t value) {
    nsf_core->WriteMemory_pAPU(0x4015, value);
}

void apu_write_joy2(uint8_t value) {
    nsf_core->WriteMemory_pAPU(0x4017, value);
}
