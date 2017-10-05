package com.simpledb

import java.io.{File, FileInputStream}
import java.nio._
import java.nio.channels.FileChannel


object FileUtil {
  
  def readNIO[T](path: String): Array[Int] = {
    val fis: FileInputStream  = new FileInputStream(new File(path))
    val channel: FileChannel = fis.getChannel()
    val bb: ByteBuffer = ByteBuffer.allocateDirect(2000 * 1024).order(ByteOrder.LITTLE_ENDIAN)

    showStats("Newly allocated read", channel, bb)
    bb.clear()

    val data = new Array[Int](channel.size().toInt / 4)
    System.out.println("File size: " + channel.size() + " bytes - Elements: " + (channel.size() / 4))

    var len: Int = channel.read(bb)
    var offset = 0
    while (len != -1) {
      bb.flip()

      bb.asIntBuffer().get(data, offset, len/4)
      offset += len/4
      bb.clear()

      len = channel.read(bb)
    }
    data
  }

  def readMMap(path: String) = {
    val file = new File(path)
    val fileSize = file.length
    val stream = new FileInputStream(file)

    val buffer = stream.getChannel.map(FileChannel.MapMode.READ_ONLY, 0, fileSize)
    buffer
  }

  def showStats(where: String, fc: FileChannel, b: Buffer) {
    System.out.println(
      where +
        " channelPosition: " +  fc.position() +
        " bufferPosition: " + b.position() +
        " limit: " + b.limit() +
        " remaining: " + b.remaining() +
        " capacity: " + b.capacity())
  }

}
