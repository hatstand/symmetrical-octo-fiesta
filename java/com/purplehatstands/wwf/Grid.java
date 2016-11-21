package com.purplehatstands.wwf;

import android.content.res.AssetManager;
import com.google.common.collect.ImmutableList;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

/** TODO: http://go/java-style#javadoc */
class Grid {

  private static final String ASSET_PATH = "model";
  private static final String WORD_LIST_PATH = "word_list";

  static {
    System.loadLibrary("foo");
  }

  private String[] words = null;

  Grid(AssetManager assetManager) {
    try {
      InputStream stream = assetManager.open(WORD_LIST_PATH, AssetManager.ACCESS_STREAMING);
      BufferedReader reader = new BufferedReader(new InputStreamReader(stream));
      String line;
      ImmutableList.Builder<String> list = ImmutableList.builder();
      while ((line = reader.readLine()) != null) {
        list.add(line);
      }
      this.words = list.build().toArray(new String[0]);
    } catch (IOException e) {
      e.printStackTrace();
    }
  }

  byte[] recogniseGrid(AssetManager assetManager, byte[] pngData) {
    return recogniseGrid(assetManager, ASSET_PATH, pngData);
  }

  byte[] recogniseRack(AssetManager assetManager, byte[] pngData) {
    return recogniseRack(assetManager, ASSET_PATH, pngData);
  }

  void solve(AssetManager assetManager, byte[] pngData) {
    solve(assetManager, ASSET_PATH, pngData, words);
  }

  private native byte[] recogniseGrid(AssetManager assetManager, String assetPath, byte[] pngData);

  private native byte[] recogniseRack(AssetManager assetManager, String assetPath, byte[] pngData);

  private native void solve(
      AssetManager assetManager, String assetPath, byte[] pngData, String[] words);
}
