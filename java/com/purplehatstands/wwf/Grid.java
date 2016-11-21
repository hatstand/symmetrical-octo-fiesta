package com.purplehatstands.wwf;

/** TODO: http://go/java-style#javadoc */
class Grid {

  static {
    System.loadLibrary("foo");
  }

  native byte[] recogniseGrid(byte[] pngData);
}
