#define SIZE_OF_DURATIONS 24

int arDurations[SIZE_OF_DURATIONS] = {
  1802, // 1, 00_trongluong.mp3
  1933, // 2, 00_xinchao.mp3
  496, // 3, 01_01.mp3
  600, // 4, 02_02.mp3
  522, // 5, 03_03.mp3
  705, // 6, 04_04.mp3
  496, // 7, 05_05.mp3
  600, // 8, 06_06.mp3
  783, // 9, 07_07.mp3
  653, // 10, 08_08.mp3
  548, // 11, 09_09.mp3
  653, // 12, 10_10.mp3
  522, // 13, 11_muoi.mp3
  522, // 14, 12_tram.mp3
  626, // 15, 13_ngan.mp3
  391, // 16, 14_chuc.mp3
  600, // 17, 15_gram.mp3
  1880, // 18, 16_calibrate.mp3
  2115, // 19, 17_put20g.mp3
  1515, // 20, 18_calibrate_done.mp3 
  1985, // 21, 19_pls_wait
  1750,  // 22, 20_pls_use
  583,  // 23, 21_linh
};

int weightPos = 0;
int helloPos = 1;
int iNumStartPos = 2;
int iTenthPos = 11;
int iDozenPos = 12;
int iHundredPos = 13;
int iThousandPos = 14;
int iGramPos = 16;
int iStartCalibPos = 17;
int iPut20gPos = 18;
int iEndCalibPos = 19;
int iPlsWaitPos = 20;
int iPlsUsePos = 21;
int iReiPos = 22;

typedef struct
{
  int fileIndex;
  int duration;
} AudioItem;

AudioItem arScript[11];
int iScriptLen = 0;

void playTrackMP3(int itemIdx, int duration) {
  myDFPlayer.play(itemIdx+1); //play mp3 file with leading identifier "0001"
  myDFPlayer.volume(20); //set volume low    
  delay(duration+10); //wait 100ms
}

void playHelloAudioScript() {

  int arIdx = 0;
  int iFileIdx = 0;
  int iDuration = 0;
  
  //hello
  arScript[arIdx++] = (AudioItem) {helloPos, arDurations[helloPos]};

  iScriptLen = arIdx;
  //play it
  for (int i = 0; i < iScriptLen; i++) {
    AudioItem item = arScript[i];
    //play item.fileIndex
    playTrackMP3(item.fileIndex, item.duration);
    //wait for item.duration
  }
}

void playCalibAudioScript(int iStep) {

  int arIdx = 0;
  int iFileIdx = 0;
  int iDuration = 0;

  //bat dau canh chinh
  if (iStep == 0) {
    arScript[arIdx++] = (AudioItem) {iStartCalibPos, arDurations[iStartCalibPos]};
  }
  else if (iStep == 1) {
    arScript[arIdx++] = (AudioItem) {iPut20gPos, arDurations[iPut20gPos]};
    arScript[arIdx++] = (AudioItem) {iPlsWaitPos, arDurations[iPlsWaitPos]};
  }
  else if (iStep == 2) {
    arScript[arIdx++] = (AudioItem) {iEndCalibPos, arDurations[iEndCalibPos]};
    arScript[arIdx++] = (AudioItem) {iPlsUsePos, arDurations[iPlsUsePos]};
  }

  iScriptLen = arIdx;
  //play it
  for (int i = 0; i < iScriptLen; i++) {
    AudioItem item = arScript[i];
    //play item.fileIndex
    playTrackMP3(item.fileIndex, item.duration);
    //wait for item.duration
  }
}

void playPlsUseAudioScript() {

  int arIdx = 0;
  int iFileIdx = 0;
  int iDuration = 0;
  
  //pls use
  arScript[arIdx++] = (AudioItem) {iPlsUsePos, arDurations[iPlsUsePos]};

  iScriptLen = arIdx;
  //play it
  for (int i = 0; i < iScriptLen; i++) {
    AudioItem item = arScript[i];
    //play item.fileIndex
    playTrackMP3(item.fileIndex, item.duration);
    //wait for item.duration
  }
}

void playWeightResultAudioScript(int w) {

  if (w < 1) return;
  
  int arIdx = 0;
  int iFileIdx = 0;
  int iDuration = 0;

  //trong luong la
  arScript[arIdx++] = (AudioItem) {weightPos, arDurations[weightPos]};

  int iMaxLevel = 3;
  int iPower = 0;
  int iWeightVal = w;
  int iUnitVal = 0;
  int iDozenVal = 0;
  int iHundredVal = 0;
  while (iWeightVal) {
    int iRemain = iWeightVal % 10;
    if (iPower == 0) {
      //hang don vi
      iUnitVal = iRemain;
    }
    else if (iPower == 1) {
      //hang dozens
      iDozenVal = iRemain;
    }
    else if (iPower == 2) {
      //hundreds
      iHundredVal = iRemain;
    }
    iWeightVal /= 10;
    if (++iPower >= iMaxLevel) {
      break;
    }
  }

  if (iHundredVal > 0) {
    int iPos = iNumStartPos+(iHundredVal-1);
    arScript[arIdx++] = (AudioItem) {iPos, arDurations[iPos]};
    arScript[arIdx++] = (AudioItem) {iHundredPos, arDurations[iHundredPos]};    
  }
  if (iDozenVal > 0) {
    if (iDozenVal > 1) {
      int iPos = iNumStartPos+(iDozenVal-1);
      arScript[arIdx++] = (AudioItem) {iPos, arDurations[iPos]};  
      arScript[arIdx++] = (AudioItem) {iDozenPos, arDurations[iDozenPos]};
    }
    else {
        //tieng viet: 'muoi' mot, 'muoi' chin
        arScript[arIdx++] = (AudioItem) {iTenthPos, arDurations[iTenthPos]};
    }
  }
  else {
    if (iHundredVal > 0) {
      //tieng viet: linh
      arScript[arIdx++] = (AudioItem) {iReiPos, arDurations[iReiPos]};
    }
  }

  if (iUnitVal > 0) {
    int iPos = iNumStartPos+(iUnitVal-1);
    arScript[arIdx++] = (AudioItem) {iPos, arDurations[iPos]};  
  }

  //gram
  arScript[arIdx++] = (AudioItem) {iGramPos, arDurations[iGramPos]};

  iScriptLen = arIdx;
  //play it
  for (int i = 0; i < iScriptLen; i++) {
    AudioItem item = arScript[i];
    //play item.fileIndex
    playTrackMP3(item.fileIndex, item.duration);
    //wait for item.duration
  }
}
