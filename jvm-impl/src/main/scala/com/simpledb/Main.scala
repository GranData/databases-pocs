package com.simpledb

import java.nio.{ByteBuffer, ByteOrder, MappedByteBuffer}


object Main extends App {
  
  val file = if (args.length < 1) {
    printf("Usage: codegen <int32-column-file>\n")
    sys.exit()
  } else {
    args.apply(0)
  }
  
  for (i <- 0 to 9) {
    // Import file
    val startRead = System.currentTimeMillis()
    val fileData = FileUtil.readMMap(file)
    val values = fileData.order(ByteOrder.LITTLE_ENDIAN)
    val durationImport: Float = System.currentTimeMillis() - startRead
    
    val (nativeRes, nativeDuration) = nativePass(values)
    values.rewind()
    val (codeGenRes, codeGenDuration) = codeGenPass(values)
  
    val timeDiff: Float = Math.abs(nativeDuration - codeGenDuration)
    val perTimeDiff = (timeDiff / ((codeGenDuration + nativeDuration) / 2)) * 100
  
    println(
      s"[$i] Rows(${values.limit()}) Bytes(${values.limit()/4}) :: " +
      s"Import Duration: ${durationImport}ms :: " +
      s"Native[$nativeRes] Duration: ${nativeDuration}ms :: " +
      s"Codegen[$codeGenRes] Duration: ${codeGenDuration}ms :: " +
      s"Diff ${timeDiff}ms ${perTimeDiff}%"
    )
  }

  def nativePass(value: ByteBuffer) = {
    val startTime = System.currentTimeMillis()
  
    var agg = 0L
    val nullValue = Int.MinValue
  
    var i = 0
    val length = value.limit() / 4
    while (i < length) {
      val v = value.getInt
      if (v != nullValue) agg += v
      i += 1
    }
  
    val duration: Float  = System.currentTimeMillis() - startTime
    (agg, duration)
  }
  
  def codeGenPass(value: ByteBuffer) = {
    val startTime = System.currentTimeMillis()
    val res = CodeGeneration
      .newSumOperator(CodeGeneration.byteBufferAlternativeImplementation)
      .compute(value)
    val duration: Float = System.currentTimeMillis() - startTime
    (res, duration)
  }
}
