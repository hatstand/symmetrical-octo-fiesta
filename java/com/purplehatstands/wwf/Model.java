package com.purplehatstands.wwf;

import android.content.res.AssetManager;

/** TODO: http://go/java-style#javadoc */
class Model {

  private static final String ASSET_PATH = "model";
  private long model;

  static {
    System.loadLibrary("foo");
  }

  Model(AssetManager assetManager) {
    model = loadModel(ASSET_PATH, assetManager);
  }

  private native long loadModel(String path, AssetManager assetManager);
}
