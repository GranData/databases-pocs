package com.simpledb

import java.nio.ByteBuffer


abstract class GeneratedComputationClass {}
abstract class GeneratedComputationArrayClass extends GeneratedComputationClass {
  def compute(references: Array[Int]): Long
}

abstract class GeneratedComputationMMapClass extends GeneratedComputationClass{
  def compute(references: ByteBuffer): Long
}



object CodeGeneration {
  import org.codehaus.janino._

  def byteBufferImplementation = {
    """
      |import java.nio.MappedByteBuffer;
      |import java.nio.ByteBuffer;
      |public class SumOperator extends com.simpledb.GeneratedComputationMMapClass {
      |  public long compute(ByteBuffer references) {
      |    long size = references.limit() / 4;
      |    int nullValue = Integer.MIN_VALUE;
      |    long acc = 0;
      |
      |    for (int i = 0; i < size; i++) {
      |       int v = references.getInt();
      |       if (v != nullValue) {
      |         acc += v;
      |       }
      |    }
      |    return acc;
      |  }
      |}
    """.stripMargin
  }
  
  def byteBufferAlternativeImplementation = {
    """
      |import java.nio.MappedByteBuffer;
      |import java.nio.ByteBuffer;
      |public class SumOperator extends com.simpledb.GeneratedComputationMMapClass {
      |  public long compute(ByteBuffer references) {
      |    long size = references.limit() / 4;
      |    int nullValue = Integer.MIN_VALUE;
      |    long acc = 0;
      |
      |    int i = 0;
      |    while (i < size) {
      |       int v = references.getInt();
      |       if (v != nullValue) {
      |         acc += v;
      |       }
      |       i++;
      |    }
      |    references.rewind();
      |    return acc;
      |  }
      |}
    """.stripMargin
  }

  def arrayImplementation = {
    """
      |public class SumOperator extends com.simpledb.GeneratedComputationArrayClass {
      |  public long compute(int[] references) {
      |    long size = references.length;
      |    int nullValue = Integer.MIN_VALUE;
      |    long acc = 0;
      |
      |    for (int i = 0; i < size; i++) {
      |       int v = references[i];
      |       if (v != nullValue) {
      |         acc += v;
      |       }
      |    }
      |    return acc;
      |  }
      |}
    """.stripMargin
  }


  private def compile(code: String): GeneratedComputationClass = {
    val compiler = new SimpleCompiler()
    compiler.setParentClassLoader(Thread.currentThread().getContextClassLoader)
    compiler.cook(code)
    compiler.getClassLoader.loadClass("SumOperator").newInstance().asInstanceOf[GeneratedComputationClass]
  }

  def newSumOperator(impl: String = byteBufferAlternativeImplementation): GeneratedComputationMMapClass = {
    val clazz: GeneratedComputationClass = compile(impl)
    clazz.asInstanceOf[GeneratedComputationMMapClass]
  }
}
