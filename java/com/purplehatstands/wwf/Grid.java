package com.purplehatstands.wwf;

import android.content.res.AssetManager;

/** TODO: http://go/java-style#javadoc */
class Grid {

  private static final String ASSET_PATH = "model";

  static {
    System.loadLibrary("foo");
  }

  byte[] recogniseGrid(AssetManager assetManager, byte[] pngData) {
    return recogniseGrid(assetManager, ASSET_PATH, pngData);
  }

  private native byte[] recogniseGrid(AssetManager assetManager, String assetPath, byte[] pngData);
}
