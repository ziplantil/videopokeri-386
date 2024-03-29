
#include "SOITTO.H"

#pragma data_seg ( "aanetpcm" );

#if SB_PCM

const unsigned long pcm_gosh_n = 5844;

const unsigned char pcm_gosh[] = {
    0x7f, 0x80, 0x82, 0x7f, 0x82, 0x7e, 0x7f, 0x81, 0x82, 0x7c, 0x87, 0x7c,
    0x62, 0xa2, 0x8b, 0x6c, 0x7b, 0x5a, 0x9a, 0x8c, 0x81, 0x83, 0x4b, 0x72,
    0xd7, 0x7e, 0x53, 0x9c, 0x54, 0x99, 0x89, 0xa0, 0x9b, 0x38, 0xbb, 0x84,
    0x48, 0xaa, 0x69, 0x7e, 0x9a, 0x68, 0x7d, 0x62, 0x7c, 0x9c, 0x54, 0x86,
    0x9d, 0x66, 0xa4, 0x58, 0x88, 0xb1, 0x49, 0x97, 0x71, 0x4d, 0xbc, 0x81,
    0x83, 0x7b, 0x46, 0xcc, 0x87, 0x6d, 0x80, 0x27, 0xaf, 0xc8, 0x80, 0x8e,
    0x4b, 0x5e, 0xaf, 0x86, 0x8a, 0x8d, 0x5e, 0xd2, 0x7b, 0x29, 0x99, 0x77,
    0x7e, 0x92, 0x80, 0x8b, 0x81, 0x75, 0x75, 0x65, 0x80, 0xa4, 0x8c, 0xa0,
    0x76, 0x29, 0x70, 0x87, 0x8d, 0xa0, 0x80, 0x72, 0x64, 0x3f, 0x6e, 0x82,
    0x94, 0xd7, 0x75, 0x65, 0x62, 0x68, 0xdf, 0x97, 0x66, 0x94, 0x5a, 0x71,
    0x9b, 0x81, 0x6c, 0x70, 0x83, 0x84, 0x6e, 0x91, 0x9d, 0x89, 0x83, 0x5d,
    0x86, 0xb2, 0xa0, 0x8f, 0x41, 0x2e, 0x93, 0xaf, 0x7c, 0x73, 0x9e, 0x66,
    0x71, 0xa0, 0x60, 0x92, 0x9e, 0x82, 0xa2, 0x4f, 0x3d, 0x7d, 0x8d, 0xad,
    0x6c, 0x30, 0x6b, 0x98, 0x94, 0x7a, 0x9b, 0x8f, 0x57, 0x8d, 0x8c, 0x7f,
    0x99, 0x83, 0x82, 0x79, 0x6e, 0x8b, 0x9d, 0x96, 0x66, 0x44, 0x89, 0x92,
    0x90, 0xc1, 0x6e, 0x5e, 0x72, 0x72, 0xbe, 0x86, 0x77, 0x91, 0x53, 0x5f,
    0x9c, 0x9c, 0x88, 0x79, 0x71, 0x82, 0x7a, 0x8d, 0x70, 0x7c, 0x97, 0x5b,
    0x69, 0x6e, 0x82, 0x93, 0x6a, 0x6e, 0x84, 0x75, 0x87, 0x93, 0x7b, 0x80,
    0x66, 0x66, 0x7e, 0xae, 0xa2, 0x75, 0x79, 0x56, 0x78, 0x8e, 0x9e, 0xab,
    0x5b, 0x50, 0x9c, 0xa3, 0x91, 0x98, 0x79, 0x69, 0x6a, 0x86, 0x8f, 0x9d,
    0x9a, 0x62, 0x64, 0x6e, 0x6f, 0xb6, 0xa0, 0x59, 0x6b, 0x74, 0x89, 0x8f,
    0x90, 0x63, 0x69, 0xa0, 0x8d, 0x69, 0x76, 0x79, 0x81, 0xa4, 0x75, 0x5f,
    0x72, 0x94, 0x90, 0x6d, 0x67, 0x6c, 0x8f, 0x82, 0x86, 0xa2, 0x70, 0x80,
    0x8c, 0x6e, 0x8b, 0x96, 0x88, 0x6f, 0x63, 0x69, 0x8f, 0x8b, 0x72, 0x83,
    0x62, 0x6e, 0xb0, 0x98, 0x86, 0x80, 0x60, 0x82, 0x93, 0x92, 0x8b, 0x82,
    0x6f, 0x5d, 0x73, 0x94, 0xa2, 0x7c, 0x66, 0x72, 0x88, 0x88, 0x8e, 0x8c,
    0x6d, 0x87, 0x8d, 0x7e, 0x79, 0x6d, 0x82, 0x8d, 0x8f, 0x7a, 0x57, 0x66,
    0x99, 0x91, 0x80, 0x83, 0x5f, 0x72, 0xb4, 0x98, 0x83, 0x7b, 0x6e, 0x97,
    0x6a, 0x80, 0x83, 0x58, 0x84, 0x8d, 0x7b, 0x7b, 0x73, 0x76, 0x83, 0x80,
    0x92, 0x7c, 0x72, 0x87, 0x6f, 0x80, 0x8b, 0x74, 0x8d, 0x7c, 0x8d, 0x98,
    0x71, 0x90, 0x81, 0x64, 0x8d, 0x79, 0x7a, 0x87, 0x6c, 0x9c, 0x8f, 0x64,
    0x90, 0x89, 0x80, 0x9a, 0x70, 0x7c, 0x80, 0x6e, 0x96, 0x68, 0x5a, 0x8a,
    0x83, 0x9b, 0x87, 0x76, 0x88, 0x6d, 0x89, 0x86, 0x5f, 0x89, 0x87, 0x79,
    0x77, 0x53, 0x77, 0x94, 0x92, 0xa3, 0x6e, 0x5a, 0x97, 0x8a, 0x8d, 0x8a,
    0x64, 0x70, 0x7b, 0x85, 0x93, 0x82, 0x7e, 0x87, 0x7a, 0x84, 0x88, 0x8f,
    0x8b, 0x72, 0x76, 0x78, 0x77, 0x87, 0x85, 0x7d, 0x85, 0x82, 0x80, 0x85,
    0x7b, 0x84, 0x88, 0x7b, 0x6e, 0x6f, 0x8a, 0x79, 0x7b, 0x86, 0x74, 0x85,
    0x85, 0x71, 0x8b, 0x8f, 0x77, 0x7f, 0x72, 0x72, 0x87, 0x81, 0x84, 0x72,
    0x78, 0x98, 0x89, 0x83, 0x80, 0x71, 0x7c, 0x7d, 0x87, 0x8b, 0x6f, 0x81,
    0x86, 0x82, 0x8a, 0x81, 0x87, 0x8b, 0x84, 0x7d, 0x74, 0x78, 0x7f, 0x7d,
    0x81, 0x7b, 0x74, 0x84, 0x88, 0x78, 0x7c, 0x89, 0x85, 0x7f, 0x80, 0x81,
    0x7d, 0x76, 0x7e, 0x83, 0x7d, 0x87, 0x84, 0x7a, 0x77, 0x76, 0x8b, 0x83,
    0x79, 0x83, 0x7b, 0x7c, 0x81, 0x82, 0x7d, 0x7f, 0x8d, 0x82, 0x79, 0x82,
    0x7a, 0x82, 0x86, 0x74, 0x7f, 0x7d, 0x7e, 0x87, 0x76, 0x7d, 0x7f, 0x83,
    0x92, 0x7e, 0x81, 0x7d, 0x7b, 0x8d, 0x79, 0x76, 0x80, 0x7c, 0x86, 0x84,
    0x76, 0x75, 0x7e, 0x88, 0x86, 0x7c, 0x78, 0x7a, 0x7f, 0x83, 0x81, 0x83,
    0x7f, 0x82, 0x87, 0x80, 0x87, 0x85, 0x7c, 0x7e, 0x76, 0x7c, 0x84, 0x7b,
    0x81, 0x79, 0x77, 0x85, 0x84, 0x8b, 0x88, 0x75, 0x7d, 0x81, 0x82, 0x87,
    0x7a, 0x81, 0x7f, 0x74, 0x80, 0x7e, 0x80, 0x86, 0x7e, 0x81, 0x7c, 0x7d,
    0x8a, 0x80, 0x7e, 0x7d, 0x78, 0x83, 0x7d, 0x7e, 0x7e, 0x78, 0x83, 0x7f,
    0x7c, 0x89, 0x7f, 0x7e, 0x7d, 0x78, 0x80, 0x80, 0x85, 0x82, 0x80, 0x78,
    0x7c, 0x87, 0x82, 0x88, 0x84, 0x7c, 0x7d, 0x80, 0x83, 0x85, 0x81, 0x78,
    0x79, 0x7b, 0x7f, 0x80, 0x86, 0x81, 0x7a, 0x83, 0x7a, 0x81, 0x87, 0x7c,
    0x7f, 0x7f, 0x7e, 0x80, 0x7e, 0x82, 0x7e, 0x7c, 0x7f, 0x7b, 0x81, 0x84,
    0x7f, 0x83, 0x7d, 0x7a, 0x86, 0x81, 0x85, 0x86, 0x7b, 0x7e, 0x7d, 0x7d,
    0x86, 0x80, 0x7d, 0x81, 0x7b, 0x7f, 0x85, 0x81, 0x81, 0x7e, 0x7d, 0x7e,
    0x7c, 0x80, 0x81, 0x81, 0x7c, 0x79, 0x80, 0x82, 0x86, 0x85, 0x7a, 0x7a,
    0x7d, 0x7f, 0x84, 0x81, 0x7e, 0x7b, 0x7f, 0x7f, 0x7f, 0x83, 0x7e, 0x7f,
    0x82, 0x80, 0x81, 0x83, 0x7f, 0x7f, 0x80, 0x7e, 0x7d, 0x7e, 0x83, 0x80,
    0x81, 0x82, 0x7c, 0x81, 0x7d, 0x7f, 0x84, 0x82, 0x80, 0x7e, 0x7f, 0x7f,
    0x80, 0x81, 0x7f, 0x7e, 0x7f, 0x7f, 0x82, 0x81, 0x7e, 0x80, 0x7e, 0x7e,
    0x81, 0x7e, 0x7e, 0x7b, 0x7b, 0x82, 0x82, 0x81, 0x80, 0x7f, 0x7d, 0x7f,
    0x81, 0x81, 0x7e, 0x7e, 0x7f, 0x83, 0x84, 0x80, 0x7e, 0x7e, 0x80, 0x80,
    0x81, 0x81, 0x7c, 0x7f, 0x82, 0x7f, 0x7f, 0x80, 0x7f, 0x7f, 0x81, 0x82,
    0x7f, 0x7d, 0x7d, 0x7c, 0x7e, 0x80, 0x7e, 0x80, 0x7f, 0x80, 0x82, 0x7e,
    0x7e, 0x7f, 0x7f, 0x81, 0x81, 0x80, 0x80, 0x7d, 0x81, 0x84, 0x7e, 0x80,
    0x82, 0x7e, 0x81, 0x81, 0x80, 0x81, 0x7d, 0x7f, 0x80, 0x7f, 0x82, 0x81,
    0x80, 0x7e, 0x7d, 0x81, 0x81, 0x80, 0x7f, 0x7d, 0x7f, 0x81, 0x7f, 0x7f,
    0x7e, 0x80, 0x81, 0x81, 0x80, 0x7c, 0x7e, 0x81, 0x80, 0x80, 0x7e, 0x7d,
    0x7f, 0x7f, 0x80, 0x80, 0x7e, 0x81, 0x81, 0x81, 0x81, 0x7e, 0x7f, 0x7e,
    0x7e, 0x7f, 0x7f, 0x7f, 0x7e, 0x7e, 0x81, 0x81, 0x80, 0x81, 0x80, 0x80,
    0x80, 0x80, 0x7f, 0x7f, 0x7f, 0x80, 0x7f, 0x7f, 0x7f, 0x80, 0x83, 0x82,
    0x80, 0x7f, 0x7f, 0x7f, 0x81, 0x80, 0x7d, 0x7e, 0x7f, 0x81, 0x7f, 0x7f,
    0x80, 0x7f, 0x81, 0x80, 0x7e, 0x7e, 0x80, 0x80, 0x7f, 0x7e, 0x7f, 0x7f,
    0x81, 0x82, 0x80, 0x7f, 0x7f, 0x81, 0x80, 0x81, 0x80, 0x7e, 0x7f, 0x7f,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x81, 0x80, 0x7f, 0x80, 0x7f, 0x80,
    0x7e, 0x7d, 0x80, 0x7f, 0x80, 0x80, 0x7e, 0x7f, 0x7f, 0x80, 0x81, 0x7f,
    0x7f, 0x7f, 0x7e, 0x80, 0x7e, 0x7e, 0x7f, 0x7e, 0x81, 0x80, 0x7e, 0x81,
    0x7f, 0x80, 0x80, 0x7d, 0x80, 0x80, 0x80, 0x82, 0x7f, 0x80, 0x80, 0x80,
    0x81, 0x80, 0x7f, 0x80, 0x7e, 0x80, 0x81, 0x7f, 0x81, 0x7f, 0x80, 0x80,
    0x80, 0x81, 0x80, 0x7f, 0x7f, 0x7e, 0x7e, 0x7f, 0x7f, 0x80, 0x7f, 0x7f,
    0x7f, 0x80, 0x81, 0x81, 0x80, 0x7f, 0x7f, 0x80, 0x80, 0x7f, 0x7f, 0x7f,
    0x7f, 0x7f, 0x7f, 0x7f, 0x80, 0x80, 0x81, 0x81, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x7f, 0x80, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x7f, 0x7e, 0x7f, 0x7f,
    0x7f, 0x80, 0x7f, 0x80, 0x7e, 0x7e, 0x7f, 0x7e, 0x7f, 0x7f, 0x7f, 0x80,
    0x80, 0x80, 0x81, 0x7f, 0x7f, 0x7f, 0x7e, 0x7f, 0x80, 0x81, 0x80, 0x7f,
    0x7f, 0x80, 0x81, 0x81, 0x80, 0x7f, 0x7f, 0x81, 0x80, 0x7f, 0x7f, 0x7f,
    0x80, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x80, 0x80, 0x80, 0x80, 0x86, 0x7e,
    0x7e, 0x7e, 0x8c, 0x82, 0x82, 0x7b, 0x5a, 0x88, 0x75, 0x88, 0x85, 0x5c,
    0x85, 0x77, 0x96, 0x8c, 0x8b, 0x81, 0x80, 0x9c, 0x8e, 0x9e, 0x7b, 0x74,
    0x5d, 0x80, 0x69, 0x7f, 0xa8, 0x5a, 0x7c, 0x52, 0x62, 0xa5, 0x87, 0x89,
    0x7e, 0x7c, 0x7d, 0x8e, 0x9c, 0x5e, 0x8b, 0x8c, 0x68, 0x83, 0x4f, 0x76,
    0x83, 0x7a, 0xaf, 0x43, 0x4e, 0xf5, 0x93, 0x93, 0xb6, 0x33, 0x77, 0x86,
    0x7a, 0xa8, 0x6b, 0x9a, 0x95, 0x54, 0x65, 0x4a, 0x93, 0xbd, 0xa6, 0x9c,
    0x53, 0x6c, 0x95, 0x69, 0x7b, 0x7c, 0x59, 0x80, 0x7a, 0x50, 0x57, 0x76,
    0xbc, 0xac, 0x88, 0x7c, 0x47, 0x81, 0xca, 0xa2, 0x5c, 0x7c, 0x31, 0x45,
    0xe1, 0x94, 0xa4, 0x8a, 0x3c, 0x83, 0x88, 0xab, 0xaf, 0x51, 0x77, 0xa4,
    0x82, 0xa2, 0x68, 0x3d, 0x6e, 0x93, 0xac, 0x78, 0x5d, 0x5b, 0x67, 0x9a,
    0x9b, 0x77, 0x90, 0x6a, 0xb7, 0xb6, 0x4b, 0x8a, 0x52, 0x81, 0xab, 0x67,
    0x8a, 0x63, 0x5c, 0xe1, 0x7c, 0x24, 0x87, 0x51, 0x8e, 0xd4, 0x89, 0x9a,
    0x36, 0x5e, 0x9a, 0x4a, 0xa9, 0x82, 0x7f, 0xb3, 0x6c, 0x84, 0x95, 0x58,
    0x97, 0x78, 0x68, 0xa7, 0x76, 0x8e, 0x58, 0x3a, 0xab, 0x7e, 0xa5, 0xb8,
    0x3d, 0x88, 0x7f, 0x57, 0x8f, 0x98, 0x67, 0x8c, 0x64, 0x5e, 0xb3, 0x9e,
    0xec, 0x95, 0x39, 0x74, 0x8a, 0xb9, 0x9b, 0x50, 0x24, 0x3d, 0x6b, 0x86,
    0x6d, 0x7a, 0xa4, 0x6e, 0xd7, 0x9d, 0x3f, 0xae, 0x8b, 0x9b, 0x7f, 0x5b,
    0x6c, 0x26, 0x94, 0x98, 0x3b, 0x87, 0x6a, 0x6e, 0xd8, 0xba, 0x72, 0x89,
    0x55, 0x94, 0x9f, 0x70, 0xa7, 0x2d, 0x70, 0xc1, 0x5b, 0xaf, 0x69, 0x43,
    0xb0, 0x70, 0xab, 0x90, 0x56, 0xa7, 0x97, 0x81, 0xa0, 0x62, 0x5c, 0x9f,
    0x8d, 0x6b, 0x63, 0x6e, 0x61, 0x8c, 0x96, 0x6f, 0x8a, 0x8c, 0x91, 0x7a,
    0x47, 0xb1, 0x7e, 0x77, 0xb2, 0x46, 0x4f, 0x34, 0x6c, 0xd7, 0xb2, 0xb8,
    0x80, 0x35, 0x68, 0x7d, 0xb7, 0xa2, 0x46, 0x76, 0x86, 0x66, 0xb1, 0x72,
    0x54, 0x7e, 0x77, 0xb4, 0xb1, 0x9a, 0x5b, 0x72, 0x69, 0x63, 0x88, 0x7f,
    0x96, 0x45, 0x4b, 0x8f, 0x6e, 0xcc, 0xb8, 0x4c, 0xde, 0x82, 0x43, 0xd6,
    0x20, 0x60, 0xc6, 0x4b, 0x91, 0x3e, 0x78, 0xff, 0x73, 0x6f, 0x63, 0x1f,
    0xa0, 0xbe, 0xb8, 0x70, 0x22, 0xb8, 0x67, 0x3f, 0xd3, 0x4a, 0x8c, 0xbd,
    0x40, 0x80, 0x3f, 0xb8, 0xd1, 0x3f, 0x81, 0x4e, 0x65, 0xf7, 0x93, 0x7e,
    0x77, 0x2b, 0xac, 0x96, 0x7c, 0xb7, 0x54, 0x80, 0xae, 0x0d, 0x5e, 0xce,
    0x8e, 0x8a, 0x72, 0x7f, 0x9c, 0x93, 0x9f, 0x71, 0x5c, 0x7e, 0xa1, 0x7a,
    0x6a, 0x66, 0x44, 0x78, 0xa3, 0x9d, 0x48, 0x95, 0x9f, 0x2e, 0x6a, 0x76,
    0x68, 0xaa, 0xdb, 0xa2, 0x61, 0x49, 0xa6, 0xb5, 0x75, 0xc8, 0x58, 0x1b,
    0xb2, 0x80, 0x54, 0x78, 0x63, 0x74, 0x80, 0x9d, 0xb5, 0x70, 0xb8, 0xd0,
    0x70, 0x95, 0x5d, 0x47, 0x85, 0x68, 0x87, 0x75, 0x54, 0x50, 0x54, 0xbb,
    0xa2, 0x95, 0x90, 0x80, 0xa9, 0xac, 0xa1, 0x64, 0x37, 0x3f, 0xaf, 0xa6,
    0x59, 0x3b, 0x26, 0xab, 0xc1, 0x7c, 0x93, 0x60, 0x7f, 0xe6, 0x95, 0x58,
    0x4d, 0x61, 0xd9, 0xab, 0x5a, 0x53, 0x2b, 0xb9, 0xeb, 0x3d, 0x4d, 0x4a,
    0x5d, 0xdc, 0xb8, 0x79, 0x28, 0x78, 0xd0, 0x6b, 0x6c, 0x72, 0x53, 0xab,
    0xc2, 0x61, 0x55, 0x5f, 0x87, 0xb1, 0x7c, 0x86, 0x64, 0x5d, 0xa1, 0x5c,
    0x77, 0x74, 0x76, 0xb4, 0x68, 0x80, 0xa1, 0x8c, 0x7c, 0x78, 0x8c, 0x8c,
    0x75, 0x58, 0x59, 0x49, 0xcd, 0xcb, 0x55, 0x79, 0x5c, 0xc9, 0xb4, 0x95,
    0x82, 0x18, 0xba, 0xe5, 0x6d, 0x29, 0x26, 0x74, 0xaa, 0x98, 0x87, 0x50,
    0x5d, 0xd9, 0x94, 0x4e, 0x89, 0x40, 0xb5, 0xad, 0x3e, 0x7b, 0x40, 0xa0,
    0xa6, 0x45, 0x72, 0x81, 0xae, 0xd6, 0x73, 0x8d, 0x9d, 0x62, 0xac, 0x6c,
    0x41, 0x8a, 0x70, 0x74, 0x74, 0x48, 0x7b, 0xa2, 0x8a, 0x9f, 0x9b, 0xbb,
    0xa5, 0x94, 0xae, 0x50, 0x6f, 0x7d, 0x5c, 0x75, 0x6d, 0x3f, 0x51, 0x44,
    0x97, 0xb9, 0x6d, 0xba, 0x50, 0x49, 0xd2, 0xcb, 0x9e, 0x9c, 0x53, 0x55,
    0x9d, 0x9b, 0x9e, 0x49, 0x83, 0x63, 0x3a, 0xb2, 0x6e, 0x80, 0x73, 0x72,
    0x96, 0x51, 0x88, 0x7c, 0x7a, 0x8e, 0x6a, 0x99, 0x98, 0x8b, 0xb4, 0x85,
    0x71, 0x82, 0x68, 0xb1, 0xcd, 0x73, 0x6f, 0x4d, 0x4b, 0x96, 0x9a, 0xce,
    0x83, 0x26, 0x65, 0x44, 0x81, 0x9e, 0x92, 0x86, 0x4f, 0xb2, 0xa5, 0xac,
    0xa4, 0x66, 0x5d, 0x8f, 0x8f, 0x42, 0x5c, 0x46, 0x7c, 0xa9, 0x87, 0x71,
    0x59, 0x86, 0x93, 0xb8, 0x9a, 0x4e, 0x6a, 0x66, 0xb3, 0xa8, 0x63, 0x4b,
    0x66, 0x98, 0xbe, 0xb8, 0x4a, 0x4e, 0x79, 0xcd, 0xd5, 0xa8, 0x64, 0x0c,
    0x4c, 0xae, 0x81, 0x87, 0x6b, 0x60, 0xac, 0x8f, 0x94, 0x77, 0x5f, 0x9d,
    0xa5, 0x9a, 0x6c, 0x39, 0x80, 0x6b, 0x81, 0x82, 0x82, 0x95, 0x80, 0x6f,
    0xad, 0xa2, 0x75, 0x8d, 0x52, 0x88, 0x68, 0x95, 0x85, 0x6c, 0x8c, 0x7e,
    0x83, 0x65, 0x5c, 0x6c, 0x8c, 0x8a, 0xa1, 0x75, 0x72, 0x5f, 0x52, 0xc0,
    0x9f, 0x96, 0x88, 0x5a, 0x64, 0x8e, 0xb9, 0x96, 0x99, 0x61, 0x7d, 0x45,
    0x6a, 0x7a, 0x42, 0x8c, 0x98, 0x91, 0x7a, 0x73, 0x8d, 0x91, 0xa4, 0xc8,
    0x78, 0x8c, 0x82, 0x7e, 0x8f, 0x71, 0x5d, 0x5e, 0x59, 0x64, 0x5c, 0x77,
    0x90, 0x68, 0x81, 0xb2, 0xbd, 0xcd, 0xb0, 0x7b, 0x7a, 0x6f, 0xa5, 0x6b,
    0x50, 0x53, 0x4e, 0x61, 0x4a, 0x62, 0x89, 0x65, 0x8c, 0x7e, 0xaa, 0x98,
    0x65, 0xc1, 0x91, 0xa1, 0x75, 0x89, 0x4f, 0x7a, 0xa8, 0x8f, 0x93, 0x48,
    0x74, 0x6c, 0x9e, 0x7c, 0x71, 0x78, 0x86, 0xb1, 0x9c, 0x9e, 0x57, 0x59,
    0x7e, 0xb5, 0xa0, 0x98, 0x82, 0x73, 0x72, 0x40, 0x71, 0x49, 0x6f, 0x71,
    0x75, 0x95, 0x6d, 0x80, 0x8a, 0xcb, 0xc2, 0x9a, 0x7c, 0x7b, 0x60, 0x57,
    0x94, 0x5b, 0x60, 0x57, 0x75, 0x78, 0x55, 0x88, 0x73, 0x8f, 0x88, 0x89,
    0xbf, 0xb2, 0xbb, 0xaa, 0x5d, 0x65, 0x82, 0xb9, 0xa2, 0x48, 0x42, 0x35,
    0x7a, 0x9d, 0x76, 0x8d, 0x7e, 0x70, 0x7f, 0x64, 0x79, 0x97, 0x9b, 0xbd,
    0x73, 0x4b, 0x79, 0x72, 0x9f, 0xc5, 0x92, 0x6e, 0x23, 0x2c, 0xa3, 0xb7,
    0xc4, 0x6e, 0x13, 0x33, 0x96, 0xee, 0xf0, 0x60, 0x16, 0x5b, 0x97, 0xf0,
    0xb6, 0x57, 0x38, 0x61, 0xa3, 0x81, 0x61, 0x3a, 0x4b, 0xa0, 0x95, 0x64,
    0x5e, 0x85, 0xb2, 0xd2, 0x8b, 0x5c, 0x42, 0x5d, 0xc4, 0xb1, 0xa2, 0x44,
    0x1e, 0x55, 0xa6, 0xe7, 0xad, 0x5a, 0x44, 0x6e, 0xab, 0xd0, 0x5b, 0x4a,
    0x4c, 0x73, 0xbf, 0x8b, 0x85, 0x52, 0x6e, 0xa9, 0x7c, 0x79, 0x88, 0x68,
    0x90, 0x84, 0x68, 0x8b, 0x75, 0x82, 0x97, 0x8a, 0x8c, 0x7f, 0x8a, 0x90,
    0x6f, 0x99, 0xa4, 0x88, 0x71, 0x6c, 0x76, 0x80, 0xb7, 0x7d, 0x51, 0x58,
    0x4d, 0x9e, 0x99, 0x7a, 0x6d, 0x50, 0x83, 0xb4, 0x98, 0x89, 0x83, 0x76,
    0x95, 0xa0, 0xa0, 0x71, 0x4d, 0x6f, 0x67, 0x8a, 0x95, 0x53, 0x73, 0x72,
    0x79, 0xae, 0x7e, 0x81, 0x7d, 0x70, 0x8f, 0x90, 0x88, 0x53, 0x53, 0x86,
    0x91, 0x90, 0x76, 0x5f, 0x75, 0xaf, 0xc6, 0x93, 0x81, 0x49, 0x6c, 0x9e,
    0xac, 0xa1, 0x4f, 0x7a, 0x6b, 0x7d, 0x72, 0x88, 0x85, 0x88, 0xa9, 0x5f,
    0x7f, 0x6c, 0x85, 0xba, 0xa7, 0x7f, 0x5b, 0x6b, 0x77, 0x91, 0x9b, 0x82,
    0x53, 0x48, 0x52, 0x7c, 0xb7, 0xa0, 0x73, 0x64, 0xa0, 0xac, 0x9a, 0x80,
    0x46, 0x3f, 0x77, 0xa9, 0x94, 0x77, 0x61, 0x67, 0x8e, 0x8d, 0x7c, 0x93,
    0x7c, 0x5e, 0x46, 0x82, 0x92, 0x9e, 0xb1, 0x76, 0x58, 0x7a, 0xbb, 0xb0,
    0xad, 0x7a, 0x83, 0x79, 0x81, 0x8a, 0x4f, 0x83, 0x77, 0x82, 0x65, 0x4b,
    0x96, 0xae, 0xba, 0x76, 0x2f, 0x68, 0xc3, 0xbd, 0x9f, 0x38, 0x25, 0x83,
    0x95, 0x93, 0x60, 0x54, 0x78, 0x8f, 0x9a, 0x6c, 0x77, 0xb5, 0x99, 0x80,
    0x4d, 0x45, 0x89, 0xa6, 0x99, 0x65, 0x44, 0x5b, 0x90, 0xc6, 0x9f, 0x72,
    0x68, 0x64, 0x93, 0xc4, 0xd7, 0x9a, 0x5a, 0x63, 0x64, 0x9d, 0xdf, 0x66,
    0x34, 0x30, 0x48, 0xac, 0xa9, 0xab, 0x71, 0x45, 0xa6, 0x82, 0x7f, 0x97,
    0x4f, 0x93, 0x73, 0x5a, 0x7e, 0x54, 0x99, 0xae, 0x8a, 0x89, 0x6a, 0x63,
    0x8e, 0x8c, 0x91, 0x90, 0x39, 0x75, 0x60, 0x95, 0xac, 0x71, 0x9a, 0x64,
    0x95, 0x9a, 0x99, 0x80, 0x7a, 0x6f, 0x7a, 0x95, 0x77, 0x83, 0x8c, 0x7d,
    0x6d, 0x64, 0x85, 0x9c, 0x7e, 0x84, 0x89, 0x86, 0x7b, 0x8d, 0x7d, 0x91,
    0x78, 0x6c, 0x6d, 0x5f, 0x81, 0x7e, 0x7b, 0x7f, 0x42, 0x67, 0x90, 0x9b,
    0xce, 0x7d, 0x70, 0x4f, 0x73, 0x91, 0xc9, 0xb8, 0x74, 0x53, 0x2f, 0x65,
    0x8a, 0xcf, 0x8d, 0x3a, 0x4a, 0x78, 0xca, 0xd9, 0x94, 0x4e, 0x33, 0xb1,
    0xdc, 0xae, 0x5b, 0x31, 0x64, 0x8a, 0xb6, 0x80, 0x65, 0x77, 0x7e, 0x8d,
    0x69, 0x5e, 0x6b, 0x8d, 0xa8, 0x87, 0x68, 0x5c, 0x94, 0x9c, 0xc1, 0x8a,
    0x4d, 0x48, 0x4d, 0x88, 0x84, 0x89, 0x68, 0x60, 0x75, 0x86, 0x9b, 0xa8,
    0xa9, 0x96, 0x8a, 0x7c, 0x6f, 0x85, 0x7c, 0x80, 0x71, 0x5e, 0x6a, 0x73,
    0x7c, 0x89, 0x84, 0x8b, 0xaf, 0x9b, 0x97, 0x73, 0x7d, 0x84, 0x8b, 0x89,
    0x79, 0x7f, 0x94, 0x8e, 0x5c, 0x67, 0x3e, 0x78, 0x91, 0xa1, 0x9c, 0x5f,
    0x6a, 0x4a, 0x68, 0xb3, 0xc2, 0x99, 0x5a, 0x49, 0x6f, 0xa0, 0x9c, 0x8c,
    0x61, 0x3f, 0x7d, 0x81, 0xad, 0x83, 0x73, 0x86, 0x65, 0x82, 0xa9, 0xa1,
    0xb6, 0xa0, 0x4b, 0x43, 0x5b, 0xb2, 0xa5, 0x8d, 0x76, 0x6f, 0x62, 0x9f,
    0x85, 0x8d, 0x97, 0x6f, 0x79, 0x2e, 0x73, 0x93, 0xb0, 0x95, 0x4e, 0x42,
    0x6a, 0xb1, 0xc0, 0x94, 0x60, 0x71, 0x74, 0x81, 0x96, 0x6e, 0x74, 0x5d,
    0x8b, 0x90, 0x57, 0x96, 0x91, 0xa4, 0xa5, 0x64, 0x7e, 0x5c, 0x6a, 0xb5,
    0x94, 0x69, 0x2d, 0x6b, 0xa6, 0xb0, 0xa2, 0x64, 0x3b, 0x95, 0xc8, 0xac,
    0x84, 0x48, 0x5f, 0x6a, 0x99, 0x9a, 0x51, 0x74, 0xa3, 0x7e, 0xa9, 0x74,
    0x80, 0x84, 0x7e, 0x9a, 0x53, 0x82, 0x7a, 0x75, 0x90, 0x78, 0x79, 0x73,
    0x6d, 0x8a, 0x91, 0x95, 0x7f, 0x62, 0x5d, 0x6d, 0x7c, 0x98, 0x8c, 0x6e,
    0x79, 0x75, 0x90, 0x8f, 0x87, 0x8a, 0x80, 0x80, 0x6f, 0x78, 0x7c, 0x80,
    0x8e, 0x79, 0x81, 0x8b, 0x9b, 0x96, 0x6e, 0x78, 0x8d, 0x74, 0x8b, 0x74,
    0x6a, 0x94, 0x8d, 0x88, 0x73, 0x7c, 0x8b, 0x8f, 0x8e, 0x77, 0x6d, 0x77,
    0x6b, 0x7f, 0x74, 0x6b, 0x7f, 0x87, 0x8d, 0x8c, 0x88, 0x7a, 0x7a, 0x7a,
    0x87, 0x7f, 0x74, 0x74, 0x75, 0x6d, 0x76, 0x82, 0x8a, 0x84, 0x84, 0x87,
    0x7e, 0x89, 0x8f, 0x86, 0x81, 0x80, 0x7a, 0x7b, 0x76, 0x78, 0x7c, 0x88,
    0x8e, 0x82, 0x7e, 0x7a, 0x86, 0x8f, 0x80, 0x81, 0x74, 0x70, 0x8f, 0x9b,
    0x8d, 0x76, 0x6f, 0x6d, 0x7b, 0x8e, 0x85, 0x73, 0x6c, 0x79, 0x82, 0x85,
    0x7d, 0x77, 0x74, 0x83, 0x91, 0x90, 0x8d, 0x73, 0x7d, 0x7c, 0x81, 0x78,
    0x6b, 0x75, 0x80, 0x8f, 0x8d, 0x80, 0x6b, 0x79, 0x8b, 0x95, 0x8c, 0x7f,
    0x7c, 0x7f, 0x7d, 0x80, 0x7d, 0x82, 0x86, 0x7f, 0x7f, 0x73, 0x77, 0x81,
    0x85, 0x7e, 0x74, 0x7f, 0x83, 0x83, 0x84, 0x7e, 0x7e, 0x83, 0x88, 0x85,
    0x85, 0x80, 0x86, 0x7b, 0x78, 0x73, 0x76, 0x84, 0x85, 0x80, 0x77, 0x77,
    0x76, 0x80, 0x81, 0x8a, 0x86, 0x82, 0x7b, 0x86, 0x8c, 0x87, 0x7e, 0x77,
    0x75, 0x7d, 0x86, 0x87, 0x7c, 0x76, 0x71, 0x76, 0x86, 0x84, 0x88, 0x81,
    0x88, 0x7a, 0x7e, 0x78, 0x84, 0x8c, 0x89, 0x7e, 0x6e, 0x7b, 0x85, 0x8d,
    0x85, 0x7d, 0x7b, 0x7a, 0x88, 0x87, 0x82, 0x81, 0x72, 0x7e, 0x78, 0x81,
    0x81, 0x86, 0x82, 0x77, 0x73, 0x83, 0x8a, 0x8f, 0x85, 0x69, 0x74, 0x76,
    0x85, 0x8c, 0x7d, 0x81, 0x7b, 0x79, 0x86, 0x7f, 0x8e, 0x8a, 0x7c, 0x78,
    0x7d, 0x7b, 0x8a, 0x80, 0x7e, 0x72, 0x76, 0x8a, 0x87, 0x83, 0x72, 0x77,
    0x7c, 0x8b, 0x83, 0x83, 0x7e, 0x7f, 0x7d, 0x7a, 0x7b, 0x87, 0x86, 0x86,
    0x77, 0x70, 0x7b, 0x84, 0x8f, 0x85, 0x7c, 0x78, 0x80, 0x7f, 0x86, 0x7d,
    0x80, 0x86, 0x84, 0x8b, 0x82, 0x7b, 0x80, 0x81, 0x7d, 0x79, 0x81, 0x82,
    0x80, 0x72, 0x74, 0x7a, 0x85, 0x8b, 0x84, 0x7f, 0x74, 0x7e, 0x80, 0x8c,
    0x83, 0x7d, 0x7c, 0x7d, 0x7d, 0x7b, 0x7d, 0x82, 0x87, 0x84, 0x83, 0x7c,
    0x7a, 0x7b, 0x7f, 0x83, 0x82, 0x88, 0x88, 0x81, 0x7a, 0x78, 0x7d, 0x82,
    0x7d, 0x80, 0x7d, 0x77, 0x80, 0x78, 0x84, 0x87, 0x79, 0x83, 0x79, 0x84,
    0x85, 0x79, 0x84, 0x7d, 0x7d, 0x87, 0x86, 0x7e, 0x84, 0x74, 0x7d, 0x81,
    0x7f, 0x8d, 0x76, 0x7d, 0x7b, 0x77, 0x89, 0x7f, 0x74, 0x7e, 0x80, 0x86,
    0x88, 0x7a, 0x7c, 0x77, 0x86, 0x90, 0x83, 0x84, 0x79, 0x76, 0x82, 0x85,
    0x82, 0x83, 0x7c, 0x82, 0x7a, 0x78, 0x7f, 0x84, 0x91, 0x7d, 0x72, 0x71,
    0x79, 0x8c, 0x8d, 0x81, 0x6e, 0x73, 0x83, 0x89, 0x88, 0x76, 0x7b, 0x83,
    0x86, 0x87, 0x7d, 0x7e, 0x81, 0x87, 0x82, 0x79, 0x7f, 0x87, 0x85, 0x78,
    0x7a, 0x70, 0x7f, 0x8b, 0x81, 0x86, 0x75, 0x7d, 0x80, 0x81, 0x8b, 0x84,
    0x83, 0x79, 0x79, 0x77, 0x7d, 0x85, 0x7e, 0x77, 0x72, 0x84, 0x81, 0x85,
    0x83, 0x77, 0x7e, 0x7f, 0x8b, 0x83, 0x81, 0x82, 0x81, 0x80, 0x85, 0x83,
    0x81, 0x7e, 0x7a, 0x7b, 0x7d, 0x86, 0x82, 0x81, 0x79, 0x82, 0x86, 0x83,
    0x82, 0x7e, 0x7e, 0x7c, 0x87, 0x7d, 0x7e, 0x7d, 0x79, 0x84, 0x7d, 0x81,
    0x79, 0x7b, 0x7f, 0x80, 0x83, 0x84, 0x81, 0x75, 0x7c, 0x7b, 0x84, 0x83,
    0x7d, 0x7f, 0x7c, 0x82, 0x82, 0x89, 0x83, 0x7f, 0x7f, 0x7a, 0x7b, 0x7a,
    0x7e, 0x7d, 0x7e, 0x7e, 0x83, 0x84, 0x87, 0x81, 0x7f, 0x7e, 0x81, 0x87,
    0x82, 0x7d, 0x73, 0x7d, 0x80, 0x83, 0x7e, 0x7d, 0x7e, 0x80, 0x86, 0x81,
    0x84, 0x7a, 0x7e, 0x86, 0x7e, 0x7d, 0x7c, 0x7f, 0x81, 0x7e, 0x82, 0x83,
    0x82, 0x83, 0x82, 0x7b, 0x74, 0x7d, 0x83, 0x82, 0x78, 0x78, 0x7e, 0x7d,
    0x8b, 0x86, 0x87, 0x81, 0x80, 0x83, 0x7d, 0x86, 0x7e, 0x7d, 0x78, 0x7a,
    0x78, 0x81, 0x8a, 0x79, 0x7e, 0x7b, 0x82, 0x86, 0x81, 0x82, 0x78, 0x80,
    0x7e, 0x81, 0x7e, 0x80, 0x84, 0x80, 0x7c, 0x76, 0x81, 0x84, 0x89, 0x79,
    0x73, 0x77, 0x80, 0x8b, 0x85, 0x81, 0x7a, 0x86, 0x8d, 0x89, 0x7f, 0x7c,
    0x76, 0x7d, 0x81, 0x75, 0x7c, 0x76, 0x83, 0x80, 0x7c, 0x83, 0x81, 0x89,
    0x8c, 0x86, 0x7b, 0x84, 0x7c, 0x82, 0x7e, 0x78, 0x7e, 0x70, 0x81, 0x7c,
    0x7e, 0x89, 0x85, 0x84, 0x7e, 0x7e, 0x7e, 0x82, 0x7b, 0x7e, 0x78, 0x7a,
    0x85, 0x85, 0x84, 0x7f, 0x79, 0x80, 0x7f, 0x7f, 0x84, 0x7b, 0x7f, 0x7d,
    0x80, 0x88, 0x88, 0x81, 0x7d, 0x7e, 0x7c, 0x80, 0x84, 0x7f, 0x77, 0x76,
    0x86, 0x83, 0x81, 0x83, 0x76, 0x7b, 0x87, 0x86, 0x87, 0x7b, 0x75, 0x81,
    0x7e, 0x80, 0x79, 0x74, 0x81, 0x84, 0x83, 0x87, 0x78, 0x7f, 0x86, 0x8a,
    0x8a, 0x77, 0x7f, 0x7b, 0x7b, 0x7f, 0x7b, 0x72, 0x7b, 0x7f, 0x86, 0x8a,
    0x81, 0x83, 0x7b, 0x83, 0x8a, 0x81, 0x83, 0x76, 0x77, 0x80, 0x84, 0x84,
    0x7a, 0x7a, 0x7c, 0x82, 0x82, 0x85, 0x7a, 0x7b, 0x80, 0x82, 0x86, 0x81,
    0x7f, 0x80, 0x80, 0x7f, 0x81, 0x80, 0x7f, 0x80, 0x7c, 0x79, 0x7d, 0x7e,
    0x7d, 0x7e, 0x7f, 0x83, 0x85, 0x81, 0x81, 0x7e, 0x84, 0x80, 0x84, 0x7c,
    0x7a, 0x7d, 0x7e, 0x83, 0x7d, 0x7e, 0x77, 0x80, 0x83, 0x87, 0x87, 0x83,
    0x80, 0x7b, 0x7e, 0x81, 0x82, 0x7e, 0x79, 0x7b, 0x7c, 0x80, 0x7d, 0x80,
    0x7e, 0x81, 0x88, 0x7e, 0x82, 0x7b, 0x7d, 0x83, 0x84, 0x82, 0x85, 0x7f,
    0x84, 0x7e, 0x7b, 0x7a, 0x7a, 0x84, 0x81, 0x81, 0x7b, 0x7d, 0x80, 0x85,
    0x81, 0x82, 0x7e, 0x7d, 0x80, 0x7f, 0x80, 0x7c, 0x7f, 0x82, 0x7b, 0x82,
    0x7e, 0x80, 0x84, 0x81, 0x85, 0x83, 0x86, 0x85, 0x80, 0x78, 0x76, 0x7a,
    0x80, 0x7e, 0x76, 0x7f, 0x7d, 0x7b, 0x84, 0x7e, 0x8e, 0x81, 0x83, 0x7c,
    0x78, 0x84, 0x7e, 0x85, 0x76, 0x77, 0x79, 0x80, 0x85, 0x82, 0x7e, 0x7c,
    0x89, 0x86, 0x86, 0x80, 0x7a, 0x82, 0x78, 0x85, 0x7e, 0x7c, 0x88, 0x79,
    0x80, 0x75, 0x7b, 0x89, 0x84, 0x80, 0x7f, 0x7f, 0x7f, 0x85, 0x7c, 0x7e,
    0x7c, 0x87, 0x8a, 0x7a, 0x7f, 0x78, 0x78, 0x88, 0x82, 0x7c, 0x7b, 0x78,
    0x86, 0x7d, 0x81, 0x7e, 0x7e, 0x85, 0x85, 0x83, 0x78, 0x7b, 0x76, 0x87,
    0x84, 0x84, 0x82, 0x70, 0x7e, 0x80, 0x87, 0x86, 0x81, 0x7e, 0x7a, 0x82,
    0x84, 0x83, 0x7a, 0x7a, 0x79, 0x82, 0x8a, 0x84, 0x7d, 0x78, 0x7b, 0x81,
    0x7e, 0x7d, 0x7c, 0x80, 0x85, 0x7a, 0x83, 0x82, 0x84, 0x86, 0x7e, 0x7e,
    0x7c, 0x7d, 0x83, 0x7a, 0x79, 0x7a, 0x82, 0x86, 0x83, 0x7f, 0x7f, 0x80,
    0x82, 0x88, 0x81, 0x84, 0x7f, 0x82, 0x7a, 0x74, 0x7c, 0x81, 0x84, 0x81,
    0x7c, 0x7f, 0x7f, 0x8a, 0x88, 0x84, 0x7a, 0x7a, 0x81, 0x7d, 0x81, 0x7b,
    0x7f, 0x78, 0x7d, 0x7d, 0x80, 0x87, 0x88, 0x84, 0x7c, 0x7e, 0x7e, 0x7c,
    0x81, 0x7d, 0x80, 0x7b, 0x7a, 0x7f, 0x7f, 0x86, 0x82, 0x7d, 0x73, 0x81,
    0x85, 0x85, 0x81, 0x77, 0x79, 0x80, 0x88, 0x87, 0x7e, 0x79, 0x7e, 0x84,
    0x83, 0x80, 0x7c, 0x7e, 0x81, 0x81, 0x80, 0x78, 0x82, 0x84, 0x84, 0x84,
    0x7e, 0x7b, 0x7e, 0x7f, 0x84, 0x7b, 0x7a, 0x7b, 0x7b, 0x83, 0x7e, 0x84,
    0x80, 0x80, 0x80, 0x84, 0x81, 0x85, 0x7f, 0x7c, 0x7f, 0x7c, 0x80, 0x7d,
    0x7d, 0x7c, 0x7f, 0x83, 0x85, 0x81, 0x82, 0x7f, 0x7f, 0x7f, 0x7e, 0x7f,
    0x7b, 0x82, 0x82, 0x81, 0x81, 0x7b, 0x7f, 0x7e, 0x84, 0x82, 0x80, 0x7e,
    0x7b, 0x7b, 0x7c, 0x83, 0x83, 0x84, 0x7f, 0x7f, 0x7d, 0x7e, 0x83, 0x7d,
    0x81, 0x7e, 0x7e, 0x7a, 0x77, 0x80, 0x7e, 0x84, 0x82, 0x7c, 0x82, 0x85,
    0x88, 0x86, 0x7f, 0x79, 0x7b, 0x82, 0x86, 0x7e, 0x79, 0x7c, 0x7a, 0x81,
    0x81, 0x82, 0x82, 0x7a, 0x84, 0x7d, 0x80, 0x82, 0x7f, 0x84, 0x7e, 0x7f,
    0x80, 0x7f, 0x80, 0x82, 0x81, 0x82, 0x80, 0x7c, 0x7e, 0x7c, 0x81, 0x7e,
    0x80, 0x82, 0x7a, 0x7c, 0x81, 0x82, 0x81, 0x7d, 0x7f, 0x7c, 0x7c, 0x87,
    0x7f, 0x80, 0x7e, 0x78, 0x80, 0x7d, 0x82, 0x81, 0x7f, 0x84, 0x7f, 0x7d,
    0x80, 0x85, 0x85, 0x85, 0x80, 0x7b, 0x7d, 0x83, 0x89, 0x7b, 0x7c, 0x77,
    0x7a, 0x82, 0x7d, 0x7f, 0x76, 0x7f, 0x83, 0x7f, 0x84, 0x84, 0x86, 0x85,
    0x84, 0x7d, 0x7f, 0x83, 0x7f, 0x7a, 0x75, 0x78, 0x7c, 0x84, 0x83, 0x7f,
    0x7d, 0x7f, 0x85, 0x85, 0x84, 0x82, 0x7d, 0x7e, 0x7c, 0x7c, 0x80, 0x7f,
    0x84, 0x80, 0x7d, 0x7d, 0x79, 0x81, 0x87, 0x84, 0x81, 0x7a, 0x79, 0x82,
    0x89, 0x8a, 0x80, 0x76, 0x77, 0x7d, 0x8b, 0x8d, 0x7d, 0x76, 0x75, 0x80,
    0x89, 0x80, 0x7a, 0x75, 0x7b, 0x87, 0x83, 0x7e, 0x77, 0x7f, 0x86, 0x84,
    0x82, 0x7b, 0x7c, 0x81, 0x83, 0x80, 0x7e, 0x7d, 0x7f, 0x7f, 0x81, 0x83,
    0x7b, 0x7f, 0x82, 0x7e, 0x83, 0x7f, 0x7d, 0x7d, 0x7e, 0x82, 0x82, 0x82,
    0x81, 0x7b, 0x7e, 0x81, 0x82, 0x86, 0x7f, 0x79, 0x7b, 0x79, 0x81, 0x87,
    0x82, 0x80, 0x78, 0x7c, 0x86, 0x86, 0x89, 0x80, 0x78, 0x7b, 0x85, 0x88,
    0x85, 0x79, 0x72, 0x7b, 0x87, 0x8a, 0x7f, 0x76, 0x79, 0x82, 0x87, 0x82,
    0x7b, 0x78, 0x80, 0x88, 0x82, 0x7d, 0x78, 0x7b, 0x83, 0x82, 0x80, 0x7a,
    0x7c, 0x85, 0x81, 0x7f, 0x7d, 0x78, 0x83, 0x83, 0x81, 0x81, 0x7b, 0x7c,
    0x81, 0x7f, 0x85, 0x7f, 0x7e, 0x7f, 0x7a, 0x82, 0x82, 0x81, 0x82, 0x78,
    0x7b, 0x83, 0x82, 0x87, 0x7f, 0x7c, 0x7e, 0x80, 0x83, 0x86, 0x82, 0x80,
    0x7b, 0x7a, 0x80, 0x83, 0x89, 0x80, 0x7a, 0x78, 0x7d, 0x8c, 0x85, 0x7c,
    0x78, 0x79, 0x82, 0x8a, 0x82, 0x78, 0x7b, 0x7b, 0x87, 0x82, 0x7d, 0x79,
    0x7a, 0x86, 0x82, 0x7c, 0x7b, 0x7d, 0x86, 0x85, 0x7e, 0x7d, 0x7e, 0x83,
    0x85, 0x80, 0x7c, 0x79, 0x7c, 0x85, 0x81, 0x80, 0x7f, 0x7e, 0x7f, 0x7c,
    0x82, 0x84, 0x82, 0x7f, 0x7b, 0x7e, 0x81, 0x83, 0x84, 0x7c, 0x78, 0x81,
    0x84, 0x85, 0x81, 0x7d, 0x7c, 0x7b, 0x7f, 0x81, 0x84, 0x84, 0x7c, 0x75,
    0x7b, 0x81, 0x88, 0x87, 0x7a, 0x77, 0x7d, 0x85, 0x89, 0x81, 0x7a, 0x7a,
    0x7e, 0x85, 0x81, 0x7a, 0x7b, 0x81, 0x81, 0x80, 0x7a, 0x80, 0x84, 0x89,
    0x84, 0x77, 0x7c, 0x81, 0x87, 0x85, 0x7c, 0x7a, 0x7b, 0x82, 0x83, 0x7f,
    0x7f, 0x7f, 0x80, 0x7f, 0x7f, 0x81, 0x83, 0x81, 0x80, 0x7c, 0x7d, 0x83,
    0x84, 0x7e, 0x7a, 0x79, 0x7d, 0x81, 0x85, 0x83, 0x7b, 0x7d, 0x7c, 0x7f,
    0x84, 0x85, 0x81, 0x79, 0x77, 0x7d, 0x83, 0x89, 0x82, 0x79, 0x77, 0x80,
    0x88, 0x88, 0x80, 0x78, 0x7b, 0x81, 0x88, 0x82, 0x7b, 0x7d, 0x7f, 0x82,
    0x7e, 0x7b, 0x7f, 0x82, 0x88, 0x7e, 0x7a, 0x7d, 0x81, 0x88, 0x83, 0x7c,
    0x7a, 0x7d, 0x81, 0x82, 0x7d, 0x7e, 0x7e, 0x80, 0x81, 0x7c, 0x80, 0x82,
    0x81, 0x83, 0x7d, 0x7e, 0x80, 0x7f, 0x7f, 0x7a, 0x7d, 0x80, 0x81, 0x84,
    0x7d, 0x7e, 0x7f, 0x81, 0x86, 0x85, 0x80, 0x7c, 0x7b, 0x7b, 0x83, 0x83,
    0x83, 0x7c, 0x79, 0x7c, 0x7c, 0x86, 0x85, 0x7e, 0x7d, 0x81, 0x84, 0x83,
    0x7f, 0x7c, 0x7b, 0x80, 0x84, 0x7f, 0x79, 0x7c, 0x80, 0x83, 0x81, 0x7c,
    0x7f, 0x7e, 0x82, 0x83, 0x80, 0x7f, 0x7f, 0x81, 0x82, 0x82, 0x80, 0x7e,
    0x7f, 0x80, 0x80, 0x81, 0x7c, 0x7a, 0x7c, 0x7e, 0x81, 0x82, 0x7f, 0x80,
    0x7e, 0x80, 0x82, 0x82, 0x82, 0x7e, 0x7f, 0x7d, 0x7c, 0x80, 0x7f, 0x7f,
    0x81, 0x7f, 0x80, 0x80, 0x80, 0x81, 0x80, 0x80, 0x80, 0x7d, 0x7f, 0x7f,
    0x7f, 0x82, 0x80, 0x7f, 0x80, 0x82, 0x84, 0x81, 0x80, 0x7c, 0x7b, 0x7f,
    0x80, 0x7e, 0x7a, 0x7c, 0x7d, 0x80, 0x81, 0x82, 0x81, 0x81, 0x84, 0x7f,
    0x82, 0x80, 0x7f, 0x7f, 0x7c, 0x7e, 0x7b, 0x7e, 0x80, 0x7f, 0x81, 0x7f,
    0x7e, 0x7f, 0x81, 0x80, 0x82, 0x80, 0x7b, 0x7f, 0x7f, 0x84, 0x82, 0x80,
    0x7e, 0x7c, 0x84, 0x83, 0x82, 0x7e, 0x7c, 0x7d, 0x80, 0x82, 0x80, 0x80,
    0x80, 0x81, 0x80, 0x7f, 0x7f, 0x7f, 0x82, 0x81, 0x7e, 0x80, 0x7c, 0x7f,
    0x82, 0x81, 0x7f, 0x7e, 0x80, 0x7e, 0x80, 0x81, 0x81, 0x7b, 0x7c, 0x7b,
    0x7e, 0x83, 0x81, 0x81, 0x7d, 0x80, 0x82, 0x7e, 0x82, 0x7c, 0x7e, 0x80,
    0x7f, 0x80, 0x79, 0x7f, 0x80, 0x84, 0x84, 0x81, 0x7f, 0x80, 0x82, 0x83,
    0x83, 0x78, 0x7c, 0x7c, 0x80, 0x82, 0x7e, 0x80, 0x7d, 0x81, 0x85, 0x81,
    0x81, 0x7e, 0x7f, 0x81, 0x7e, 0x7f, 0x7e, 0x7e, 0x81, 0x7f, 0x7e, 0x7d,
    0x7d, 0x81, 0x7f, 0x82, 0x81, 0x7f, 0x7e, 0x81, 0x82, 0x83, 0x7e, 0x7e,
    0x7e, 0x7d, 0x81, 0x80, 0x80, 0x7d, 0x7c, 0x7e, 0x80, 0x83, 0x83, 0x7f,
    0x80, 0x81, 0x81, 0x83, 0x84, 0x7e, 0x7e, 0x7a, 0x7d, 0x7e, 0x7f, 0x82,
    0x7b, 0x7c, 0x80, 0x85, 0x87, 0x83, 0x7f, 0x7d, 0x7d, 0x82, 0x81, 0x7b,
    0x79, 0x7c, 0x7e, 0x81, 0x7e, 0x7e, 0x7f, 0x82, 0x85, 0x80, 0x80, 0x7d,
    0x7f, 0x84, 0x83, 0x80, 0x7c, 0x7b, 0x7f, 0x7f, 0x84, 0x7f, 0x7b, 0x7c,
    0x81, 0x85, 0x83, 0x82, 0x7e, 0x7d, 0x80, 0x85, 0x82, 0x7f, 0x7c, 0x7c,
    0x7d, 0x80, 0x7f, 0x7f, 0x7f, 0x82, 0x81, 0x80, 0x80, 0x80, 0x80, 0x84,
    0x85, 0x80, 0x7d, 0x7c, 0x7e, 0x7d, 0x81, 0x7d, 0x7b, 0x7d, 0x83, 0x84,
    0x83, 0x7f, 0x7c, 0x7c, 0x82, 0x85, 0x83, 0x7e, 0x7a, 0x7d, 0x7d, 0x80,
    0x7d, 0x7e, 0x80, 0x81, 0x81, 0x82, 0x82, 0x7f, 0x7f, 0x83, 0x81, 0x80,
    0x80, 0x7f, 0x7e, 0x7d, 0x80, 0x7d, 0x7f, 0x7f, 0x83, 0x80, 0x7f, 0x7e,
    0x7c, 0x7e, 0x82, 0x84, 0x82, 0x7e, 0x7f, 0x7d, 0x7f, 0x81, 0x7e, 0x80,
    0x7f, 0x81, 0x7f, 0x80, 0x7f, 0x7f, 0x7e, 0x81, 0x81, 0x82, 0x80, 0x82,
    0x7d, 0x7c, 0x7d, 0x80, 0x81, 0x82, 0x80, 0x7c, 0x80, 0x7d, 0x81, 0x7f,
    0x81, 0x80, 0x7e, 0x7d, 0x7f, 0x7d, 0x81, 0x7f, 0x80, 0x80, 0x81, 0x80,
    0x80, 0x83, 0x80, 0x80, 0x7e, 0x81, 0x7e, 0x80, 0x80, 0x7f, 0x7d, 0x7d,
    0x7f, 0x7f, 0x7f, 0x80, 0x7d, 0x7e, 0x7e, 0x82, 0x81, 0x7e, 0x80, 0x7f,
    0x80, 0x7f, 0x83, 0x80, 0x82, 0x7e, 0x80, 0x7f, 0x81, 0x81, 0x7e, 0x80,
    0x80, 0x7d, 0x7e, 0x7f, 0x7f, 0x7f, 0x80, 0x82, 0x81, 0x80, 0x7f, 0x7f,
    0x80, 0x81, 0x7e, 0x7d, 0x7d, 0x7f, 0x7f, 0x80, 0x7e, 0x7f, 0x7e, 0x83,
    0x82, 0x82, 0x81, 0x7d, 0x80, 0x7f, 0x82, 0x81, 0x81, 0x80, 0x7f, 0x7d,
    0x7d, 0x7e, 0x7e, 0x7e, 0x81, 0x7f, 0x7f, 0x7e, 0x80, 0x82, 0x7e, 0x81,
    0x7f, 0x7f, 0x80, 0x82, 0x7e, 0x7c, 0x7d, 0x7e, 0x82, 0x81, 0x82, 0x7e,
    0x7e, 0x82, 0x7e, 0x80, 0x80, 0x81, 0x82, 0x7e, 0x7f, 0x7d, 0x7e, 0x82,
    0x81, 0x80, 0x7d, 0x7e, 0x7f, 0x80, 0x7f, 0x7f, 0x80, 0x7f, 0x80, 0x7e,
    0x7f, 0x80, 0x80, 0x81, 0x80, 0x82, 0x80, 0x81, 0x82, 0x80, 0x7e, 0x7e,
    0x81, 0x7e, 0x7e, 0x7f, 0x7c, 0x7e, 0x7d, 0x80, 0x81, 0x81, 0x80, 0x7d,
    0x81, 0x82, 0x83, 0x81, 0x80, 0x7e, 0x7e, 0x81, 0x7f, 0x7e, 0x7d, 0x7f,
    0x7f, 0x81, 0x81, 0x7d, 0x7e, 0x7f, 0x82, 0x82, 0x81, 0x81, 0x7f, 0x7f,
    0x7d, 0x7d, 0x80, 0x81, 0x82, 0x7e, 0x7b, 0x7d, 0x7e, 0x82, 0x7f, 0x80,
    0x7f, 0x80, 0x81, 0x81, 0x80, 0x7d, 0x80, 0x81, 0x81, 0x80, 0x81, 0x80,
    0x81, 0x7f, 0x7c, 0x7d, 0x80, 0x81, 0x80, 0x7d, 0x7d, 0x7d, 0x81, 0x83,
    0x82, 0x82, 0x7f, 0x7f, 0x7e, 0x82, 0x82, 0x81, 0x7f, 0x7c, 0x7c, 0x7e,
    0x80, 0x80, 0x7f, 0x81, 0x7e, 0x7f, 0x81, 0x81, 0x82, 0x80, 0x82, 0x7e,
    0x7f, 0x7e, 0x80, 0x81, 0x7f, 0x7f, 0x7b, 0x7f, 0x80, 0x80, 0x81, 0x7d,
    0x80, 0x7f, 0x81, 0x82, 0x82, 0x81, 0x7d, 0x7d, 0x7d, 0x81, 0x81, 0x82,
    0x80, 0x7d, 0x7e, 0x7c, 0x80, 0x81, 0x82, 0x83, 0x7d, 0x7e, 0x7e, 0x80,
    0x84, 0x80, 0x80, 0x7d, 0x7d, 0x81, 0x82, 0x80, 0x7f, 0x7d, 0x7e, 0x80,
    0x80, 0x81, 0x7e, 0x7e, 0x7f, 0x7d, 0x81, 0x83, 0x82, 0x82, 0x7d, 0x7e,
    0x7f, 0x81, 0x82, 0x80, 0x81, 0x7e, 0x7b, 0x7d, 0x7e, 0x81, 0x82, 0x80,
    0x7e, 0x7d, 0x80, 0x84, 0x84, 0x80, 0x7e, 0x7c, 0x80, 0x82, 0x81, 0x7e,
    0x7c, 0x7e, 0x80, 0x80, 0x7f, 0x7e, 0x80, 0x80, 0x81, 0x80, 0x7f, 0x81,
    0x7f, 0x80, 0x7e, 0x7e, 0x7f, 0x80, 0x80, 0x81, 0x80, 0x7e, 0x7d, 0x7e,
    0x81, 0x81, 0x82, 0x7f, 0x7d, 0x7f, 0x7f, 0x80, 0x81, 0x80, 0x7f, 0x80,
    0x81, 0x81, 0x7e, 0x7e, 0x7e, 0x81, 0x82, 0x80, 0x7e, 0x7d, 0x7e, 0x7f,
    0x81, 0x80, 0x80, 0x80, 0x7f, 0x81, 0x80, 0x80, 0x80, 0x7f, 0x80, 0x80,
    0x80, 0x7f, 0x80, 0x7e, 0x7d, 0x7e, 0x7f, 0x82, 0x81, 0x80, 0x7f, 0x80,
    0x80, 0x80, 0x80, 0x81, 0x80, 0x80, 0x7e, 0x7e, 0x7f, 0x81, 0x80, 0x7f,
    0x7e, 0x7d, 0x80, 0x81, 0x82, 0x80, 0x81, 0x7f, 0x7e, 0x7f, 0x80, 0x81,
    0x81, 0x80, 0x7f, 0x7e, 0x7e, 0x7f, 0x80, 0x81, 0x7f, 0x7e, 0x7d, 0x7f,
    0x7e, 0x80, 0x81, 0x80, 0x7f, 0x7e, 0x82, 0x82, 0x81, 0x7f, 0x7e, 0x80,
    0x7f, 0x81, 0x7f, 0x7e, 0x7e, 0x80, 0x80, 0x7f, 0x81, 0x7f, 0x80, 0x81,
    0x7f, 0x7e, 0x7f, 0x81, 0x81, 0x80, 0x80, 0x7e, 0x7e, 0x7e, 0x83, 0x81,
    0x7f, 0x7e, 0x7d, 0x80, 0x81, 0x82, 0x7f, 0x7e, 0x7f, 0x7f, 0x81, 0x81,
    0x7f, 0x7e, 0x7f, 0x7f, 0x80, 0x7e, 0x7f, 0x80, 0x80, 0x81, 0x7f, 0x7f,
    0x80, 0x7f, 0x80, 0x81, 0x7e, 0x7f, 0x7f, 0x80, 0x80, 0x81, 0x7f, 0x7f,
    0x7d, 0x7f, 0x82, 0x81, 0x81, 0x7e, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x7e,
    0x7f, 0x80, 0x80, 0x82, 0x7f, 0x7f, 0x81, 0x81, 0x80, 0x7e, 0x7c, 0x80,
    0x7f, 0x80, 0x81, 0x7d, 0x7f, 0x7f, 0x82, 0x82, 0x80, 0x80, 0x80, 0x7f,
    0x80, 0x7f, 0x7e, 0x80, 0x7e, 0x80, 0x81, 0x7f, 0x7f, 0x7f, 0x80, 0x81,
    0x7f, 0x80, 0x7e, 0x7d, 0x81, 0x80, 0x81, 0x7f, 0x7e, 0x7f, 0x81, 0x81,
    0x80, 0x7f, 0x7e, 0x81, 0x80, 0x80, 0x7f, 0x7f, 0x7f, 0x7f, 0x80, 0x7f,
    0x80, 0x81, 0x80, 0x7e, 0x7e, 0x80, 0x80, 0x80, 0x81, 0x80, 0x7f, 0x7f,
    0x80, 0x7f, 0x81, 0x81, 0x7e, 0x7d, 0x7c, 0x7f, 0x81, 0x81, 0x82, 0x7e,
    0x7d, 0x80, 0x80, 0x81, 0x80, 0x7f, 0x7f, 0x7e, 0x7f, 0x80, 0x7f, 0x80,
    0x80, 0x7f, 0x7e, 0x7e, 0x80, 0x81, 0x81, 0x82, 0x7f, 0x7d, 0x7e, 0x80,
    0x82, 0x81, 0x7f, 0x7c, 0x7d, 0x81, 0x83, 0x81, 0x7f, 0x7d, 0x7e, 0x81,
    0x81, 0x7f, 0x7e, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x80, 0x81, 0x82, 0x7f,
    0x7f, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x7d, 0x7e, 0x7f, 0x81, 0x82, 0x82,
    0x7e, 0x7c, 0x7e, 0x82, 0x83, 0x82, 0x7e, 0x7d, 0x7e, 0x7e, 0x82, 0x7f,
    0x80, 0x7e, 0x7e, 0x80, 0x7f, 0x81, 0x81, 0x80, 0x80, 0x80, 0x7f, 0x7f,
    0x80, 0x7f, 0x7f, 0x7e, 0x7f, 0x7f, 0x7f, 0x81, 0x80, 0x80, 0x7f, 0x7e,
    0x7f, 0x81, 0x82, 0x81, 0x7d, 0x7e, 0x7d, 0x81, 0x80, 0x7f, 0x80, 0x7f,
    0x7f, 0x80, 0x81, 0x7f, 0x80, 0x7f, 0x80, 0x80, 0x81, 0x80, 0x7f, 0x7e,
    0x7e, 0x7e, 0x80, 0x80, 0x81, 0x7f, 0x80, 0x80, 0x7e, 0x80, 0x7f, 0x81,
    0x81, 0x80, 0x7e, 0x7d, 0x7e, 0x80, 0x80, 0x81, 0x80, 0x7f, 0x7e, 0x81,
    0x81, 0x81, 0x7f, 0x7e, 0x7f, 0x7f, 0x82, 0x80, 0x80, 0x7c, 0x7d, 0x7f,
    0x82, 0x82, 0x81, 0x7e, 0x7e, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x81, 0x7f,
    0x7f, 0x7f, 0x7e, 0x81, 0x81, 0x80, 0x7f, 0x7f, 0x7f, 0x82, 0x7f, 0x7f,
    0x7e, 0x7f, 0x80, 0x81, 0x80, 0x7f, 0x7e, 0x7f, 0x80, 0x7f, 0x82, 0x80,
    0x80, 0x7e, 0x7f, 0x7f, 0x80, 0x80, 0x80, 0x7e, 0x7f, 0x80, 0x80, 0x81,
    0x80, 0x7f, 0x80, 0x80, 0x80, 0x7f, 0x7e, 0x80, 0x80, 0x80, 0x80, 0x7f,
    0x7f, 0x80, 0x7f, 0x7e, 0x7e, 0x81, 0x80, 0x7f, 0x7d, 0x7f, 0x7f, 0x80,
    0x81, 0x81, 0x7f, 0x7e, 0x81, 0x7f, 0x81, 0x7f, 0x7f, 0x80, 0x80, 0x7f,
    0x7e, 0x7f, 0x80, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
    0x81, 0x81, 0x80, 0x80, 0x7e, 0x7e, 0x7f, 0x80, 0x80, 0x81, 0x7f, 0x7f,
    0x7e, 0x80, 0x81, 0x81, 0x81, 0x7f, 0x80, 0x80, 0x81, 0x7f, 0x7d, 0x7d,
    0x7f, 0x82, 0x81, 0x80, 0x7d, 0x7e, 0x80, 0x81, 0x81, 0x80, 0x80, 0x7f,
    0x7f, 0x7f, 0x7e, 0x7f, 0x80, 0x81, 0x80, 0x7e, 0x7e, 0x7e, 0x7f, 0x81,
    0x81, 0x80, 0x7f, 0x7f, 0x7f, 0x7f, 0x80, 0x7f, 0x80, 0x7f, 0x80, 0x80,
    0x80, 0x7f, 0x7f, 0x7f, 0x80, 0x80, 0x7e, 0x7e, 0x7e, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x81, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x80,
    0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x80, 0x7e, 0x80, 0x80, 0x81,
    0x81, 0x7f, 0x80, 0x7f, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
    0x7e, 0x7f, 0x7e, 0x80, 0x82, 0x80, 0x80, 0x7d, 0x7e, 0x80, 0x81, 0x81,
    0x80, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7e, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
    0x80, 0x80, 0x7f, 0x80, 0x80, 0x81, 0x80, 0x7e, 0x7e, 0x7e, 0x80, 0x81,
    0x80, 0x80, 0x7e, 0x7f, 0x7f, 0x7f, 0x7f, 0x81, 0x81, 0x81, 0x7f, 0x7e,
    0x7f, 0x81, 0x81, 0x80, 0x7f, 0x7e, 0x7e, 0x7f, 0x80, 0x80, 0x7f, 0x7f,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x80, 0x81, 0x81, 0x7f,
    0x7e, 0x7e, 0x80, 0x82, 0x80, 0x7f, 0x7e, 0x7e, 0x7f, 0x80, 0x80, 0x81,
    0x7f, 0x7e, 0x7e, 0x7e, 0x80, 0x80, 0x81, 0x7f, 0x7e, 0x7f, 0x7f, 0x80,
    0x80, 0x80, 0x7f, 0x7f, 0x7f, 0x80, 0x80, 0x7f, 0x7f, 0x7e, 0x7e, 0x7f,
    0x81, 0x82, 0x7f, 0x7c, 0x7e, 0x80, 0x83, 0x82, 0x7f, 0x7e, 0x7e, 0x81,
    0x81, 0x81, 0x7e, 0x7f, 0x80, 0x81, 0x80, 0x7e, 0x7f, 0x80, 0x81, 0x80,
    0x7f, 0x7f, 0x7f, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x81, 0x7f, 0x7f, 0x7f,
    0x7f, 0x7f, 0x7f, 0x80, 0x80, 0x7f, 0x7e, 0x7e, 0x7f, 0x7f, 0x81, 0x80,
    0x7f, 0x7e, 0x7e, 0x7f, 0x80, 0x80, 0x80, 0x7f, 0x7e, 0x7e, 0x7f, 0x80,
    0x81, 0x7f, 0x7f, 0x7f, 0x80, 0x81, 0x80, 0x7f, 0x7d, 0x7f, 0x81, 0x81,
    0x7f, 0x7e, 0x7e, 0x80, 0x81, 0x80, 0x7f, 0x7f, 0x80, 0x80, 0x7f, 0x7e,
    0x7e, 0x80, 0x80, 0x80, 0x7e, 0x7e, 0x7f, 0x81, 0x81, 0x80, 0x7e, 0x7e,
    0x80, 0x80, 0x80, 0x7f, 0x80, 0x7f, 0x80, 0x80, 0x80, 0x81, 0x80, 0x80,
    0x7f, 0x80, 0x80, 0x80, 0x80, 0x7f, 0x7e, 0x7f, 0x7f, 0x80, 0x80, 0x7f,
    0x7f, 0x7e, 0x7f, 0x80, 0x80, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x80, 0x81,
    0x80, 0x80, 0x7e, 0x7f, 0x7f, 0x7f, 0x80, 0x80, 0x7f, 0x7f, 0x7e, 0x7f,
    0x7f, 0x7f, 0x80, 0x80, 0x80, 0x7e, 0x7f, 0x7f, 0x7f, 0x80, 0x7f, 0x7f,
    0x7f, 0x7f, 0x80, 0x80, 0x80, 0x80, 0x7f, 0x80, 0x7f, 0x7f, 0x7f, 0x7f,
    0x80, 0x7f, 0x7f, 0x80, 0x7f, 0x80, 0x80, 0x7f, 0x7f, 0x7f, 0x80, 0x80,
    0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x80, 0x81,
    0x81, 0x80, 0x7f, 0x7f, 0x80, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x7f, 0x7f,
    0x7f, 0x7f, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x7f, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x80, 0x80, 0x80,
    0x80, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x80, 0x7f, 0x80, 0x7f, 0x80, 0x80,
    0x7f, 0x80, 0x80, 0x80, 0x80, 0x7f, 0x80, 0x7f, 0x7f, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x7f, 0x80, 0x80, 0x7f, 0x7f, 0x80, 0x7f, 0x80, 0x80,
    0x7f, 0x7f, 0x7f, 0x80, 0x7f, 0x80, 0x7f, 0x7f, 0x80, 0x80, 0x80, 0x7f,
    0x80, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x7f,
    0x7f, 0x80, 0x80, 0x7f, 0x7f, 0x7f, 0x7f, 0x80, 0x80, 0x7f, 0x7e, 0x7e,
    0x80, 0x80, 0x80, 0x80, 0x7f, 0x80, 0x7f, 0x80, 0x7f, 0x7f, 0x7f, 0x7f,
    0x7f, 0x7f, 0x7f, 0x80, 0x80, 0x7f, 0x7f, 0x7f, 0x81, 0x80, 0x80, 0x7f,
    0x7f, 0x7f, 0x80, 0x80, 0x80, 0x7f, 0x80, 0x80, 0x80, 0x80, 0x80, 0x81,
    0x81, 0x80, 0x7f, 0x7f, 0x7f, 0x80, 0x81, 0x81, 0x80, 0x7f, 0x7f, 0x80,
    0x80, 0x81, 0x7f, 0x7f, 0x7f, 0x80, 0x82, 0x81, 0x81, 0x7f, 0x7f, 0x80,
    0x81, 0x81, 0x81, 0x7f, 0x80, 0x7f, 0x80, 0x7f, 0x7f, 0x80, 0x80, 0x7f,
    0x7f, 0x7f, 0x7f, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x7f, 0x80, 0x7f, 0x80,
    0x7f, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x80, 0x81, 0x80,
    0x7f, 0x7e, 0x7f, 0x80, 0x81, 0x80, 0x80, 0x80, 0x80, 0x81, 0x82, 0x81,
    0x80, 0x7f, 0x81, 0x81, 0x81, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x81, 0x80, 0x80, 0x80, 0x7f, 0x80, 0x80, 0x7f, 0x80, 0x80, 0x80
};

#endif
