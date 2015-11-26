/*********************************************************************************
 *
 * Lossy decoder with 4-point DCT, midtread quantizer, DC prediction, 
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
 *********************************************************************************/
#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "codeclib.h"

void usage() {
    cout << "Video decoder with motion est, Golomb-Rice code, and arithmetic code:" << endl;;
    cout << "Usage: viddec infile outfile" << endl;
    cout << "    infile:  Input compressed file." << endl;
    cout << "    outfile: Output yuv file." << endl;
}

int main(int argc,char **argv) {
 
    ifstream ifsInfile;
    ofstream ofsOutfile;
    int iWidth, iHeight, iTotalBytes, iFrames;
    unsigned char *pcImgBuf, *pcBitstreamBuf;
    float fQstep;

    if(argc < 3) {
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

    //read width, height, total bytes
	ifsInfile.read((char *)&iWidth, sizeof(int));
	ifsInfile.read((char *)&iHeight, sizeof(int));
	ifsInfile.read((char *)&iFrames, sizeof(int));
	ifsInfile.read((char *)&fQstep, sizeof(float));

    int iImageArea = iWidth * iHeight * 3 / 2;
    pcImgBuf = new unsigned char[iImageArea];
    if (!pcImgBuf) {
        cout << "Fail to create image buffer." << endl;
        return -5;
    }

    pcBitstreamBuf = new unsigned char[iImageArea];
    if (!pcBitstreamBuf) {
        cout << "Fail to create output buffer." << endl;
        return -7;
    }

    IDecoder *pDecoder = new IDecoder(iWidth, iHeight);

    for (int i = 0; i < iFrames; i++) {
        cout << ".";
        ifsInfile.read((char *)&iTotalBytes, sizeof(int));

        ifsInfile.read((char *)pcBitstreamBuf, iTotalBytes);

        int iUsedBytes = pDecoder->decodeImage(i == 0, pcBitstreamBuf, fQstep);

        pDecoder->GetImage(pcImgBuf);
    
        ofsOutfile.write((const char *)pcImgBuf, iImageArea);
    }
    cout << endl;

    ifsInfile.close();
    ofsOutfile.close();

    delete pDecoder;
    delete pcImgBuf;
    delete pcBitstreamBuf;

    return 0;
}