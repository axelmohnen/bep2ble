void SendCANFramesToSerial(aData)
{
  byte buf[8];

  // build & send CAN frames to RealDash.
  // a CAN frame payload is always 8 bytes containing data in a manner
  // described by the RealDash custom channel description XML file
  // all multibyte values are handled as little endian by default.
  // endianess of the values can be specified in XML file if it is required to use big endian values

  // build 1. CAN frame
  memcpy(buf, aData[0], 2);
  memcpy(buf + 2, aData[1], 2);
  memcpy(buf + 4, aData[2], 2);
  memcpy(buf + 6, aData[3], 2);

  // write 1. CAN frame to serial
  SendCANFrameToSerial(3200, buf);
  
   // build 2. CAN frame
  memcpy(buf, aData[4], 2);
  memcpy(buf + 2, aData[5], 2);
  memcpy(buf + 4, aData[6], 2);
  memcpy(buf + 6, aData[7], 2);

  // write 2. CAN frame to serial
  SendCANFrameToSerial(3201, buf);
  
  // build 3. CAN frame
  memcpy(buf, aData[8], 2);
  memcpy(buf + 2, aData[9], 2);
  memcpy(buf + 4, aData[10], 2);
  memcpy(buf + 6, aData[11], 2);

  // write 3. CAN frame to serial
  SendCANFrameToSerial(3202, buf);
}


void SendCANFrameToSerial(unsigned long canFrameId, const byte* frameData)
{
  // the 4 byte identifier at the beginning of each CAN frame
  // this is required for RealDash to 'catch-up' on ongoing stream of CAN frames
  const byte serialBlockTag[4] = { 0x44, 0x33, 0x22, 0x11 };
  Serial.write(serialBlockTag, 4);

  // the CAN frame id number (as 32bit little endian value)
  Serial.write((const byte*)&canFrameId, 4);

  // CAN frame payload
  Serial.write(frameData, 8);
}
