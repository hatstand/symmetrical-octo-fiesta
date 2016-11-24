package com.purplehatstands.wwf.cheating;

import android.content.res.AssetManager;
import com.google.common.collect.ImmutableList;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

/** TODO: http://go/java-style#javadoc */
class Grid {

  private static final String MODEL_PATH = "model";
  private static final String WORD_LIST_PATH = "word_list";

  static {
    System.loadLibrary("cheating");
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
    return recogniseGrid(assetManager, MODEL_PATH, pngData);
  }

  byte[] recogniseRack(AssetManager assetManager, byte[] pngData) {
    return recogniseRack(assetManager, MODEL_PATH, pngData);
  }

  String solve(AssetManager assetManager, byte[] pngData) {
    return solve(assetManager, MODEL_PATH, pngData, words);
  }

  private native byte[] recogniseGrid(AssetManager assetManager, String assetPath, byte[] pngData);

  private native byte[] recogniseRack(AssetManager assetManager, String assetPath, byte[] pngData);

  private native String solve(
      AssetManager assetManager, String assetPath, byte[] pngData, String[] words);
}
