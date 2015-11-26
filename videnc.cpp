/*********************************************************************************
 *
 * A simple video codec:
 * Motion estimation, DCT, midtread quantizer, 
 * Golomb-Rice code, and binary arithmetic code.
 *
 * The arithmetic codec was originally written by Dr. Chengjie Tu 
 * at the Johns Hopkins University (now with Microsoft Windows Media Division).
 *
 * Modified by Jie Liang
 * School of Engineering Science
 * Simon Fraser University
 *
 * Feb. 2005
 *
 * To do:
 * 1. Intra MB in P frame: criterion?
 * 2. Better entropy coding: zigzag scanning, early termination.
 * 3. CBP
 *
 *********************************************************************************/

#include <iostream>
#include <fstream>
#include <iomanip> 
using namespace std;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "codeclib.h"

void usage() {
    cout << "A simple video codec:" << endl;;
    cout << "Usage: videnc infile outfile width height frames qstep" << endl;
    cout << "    infile: Input video sequence." << endl;
    cout << "    outfile:    Output compressed file." << endl;
    cout << "    width: Frame width" << endl;
    cout << "    height: Frame height" << endl;
    cout << "    frames: Number of frames to be encoded (from the first frame)" << endl;
    cout << "    qstep:      Quantization step size (can be floating-point)" << endl;
}

//--------------------------------------------------------
//
// main routine
//
//--------------------------------------------------------
int main(int argc,char **argv) {
 
    ifstream ifsInfile;
    ofstream ofsOutfile;

    int iWidth, iHeight, iFrames, iOutBytes;
    int iGRPara = 0, iAllBytes = 0;
    unsigned char *pcImgBuf, *pcBitstreamBuf;
    float fQstep;

    if(argc < 7) {
        usage();
        return -1;
    }

    //open input file
    ifsInfile.open(argv[1], ios::in|ios::binary);
    if(!ifsInfile)
    {
        cout << "Can't open file " << argv[1] << endl;
        return -2;
    }

    //open output file
    ofsOutfile.open(argv[2], ios::out|ios::binary);
    if(!ofsOutfile)
    {
        cout << "Can't open file " << argv[2] << endl;
        return -3;
    }

    iWidth = atoi(argv[3]);
    iHeight = atoi(argv[4]);
    iFrames = atoi(argv[5]);

    fQstep = (float) atof(argv[6]);

    int iFrameSize = iWidth * iHeight * 3 / 2; //for YUV
    pcImgBuf = new unsigned char[iFrameSize];
    if (!pcImgBuf) {
        cout << "Fail to create image buffer." << endl;
        return -5;
    }

    pcBitstreamBuf = new unsigned char[iFrameSize];
    if (!pcBitstreamBuf) {
        cout << "Fail to create output buffer." << endl;
        return -6;
    }

    IEncoder *pEncoder = new IEncoder(iWidth, iHeight);

#ifdef DUMPMV
    ofstream ofsDump;
    ofsDump.open("mv.dat", ios::out|ios::binary);
#endif

    //write file header
    ofsOutfile.write((const char *) &iWidth, sizeof(int));
    ofsOutfile.write((const char *) &iHeight, sizeof(int));
    ofsOutfile.write((const char *) &iFrames, sizeof(int));
    ofsOutfile.write((const char *) &fQstep, sizeof(float));

    for (int i = 0; i < iFrames; i++) {
        //read one frame
        ifsInfile.read((char *)pcImgBuf, iFrameSize);
        pEncoder->SetImage(pcImgBuf);

        //main routine to encode the frame
        iOutBytes = pEncoder->codeImage(i == 0, pcBitstreamBuf, fQstep);
        iAllBytes += iOutBytes;

        cout << iOutBytes << endl;
 
        //write one frame data to output file
        ofsOutfile.write((const char *) &iOutBytes, sizeof(int));
        ofsOutfile.write((const char *) pcBitstreamBuf, iOutBytes);

#ifdef DUMPMV
        if (i > 0) {
            pEncoder->DumpMV(ofsDump);
        }
#endif

    }

    cout << "Bits/pixel: " << std::setprecision(5) << iAllBytes * 8.0f / (iWidth * iHeight * iFrames) << endl;

    ifsInfile.close();
    ofsOutfile.close();

#ifdef DUMPMV
    ofsDump.close();
#endif

    delete pEncoder;
    delete pcImgBuf;
    delete pcBitstreamBuf;

    return 0;
}