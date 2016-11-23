package com.purplehatstands.wwf;

import com.google.common.io.Files;
import java.io.File;
import java.io.IOException;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;

/** TODO: http://go/java-style#javadoc */
class BuildModel {

  static {
    System.loadLibrary("jni");
  }

  private static class ImageFrame extends JFrame {
    ImageFrame(byte[] pngData) {
      ImageIcon image = new ImageIcon(pngData);
      JPanel panel = new JPanel();
      panel.setSize(200, 200);
      JLabel label = new JLabel();
      label.setIcon(image);
      panel.add(label);
      getContentPane().add(panel);
    }
  }

  // Try to recognise everything in a screenshot.
  private native void recognise(byte[] pngData);

  // Called when we cannot recognise a particular tile, in which case we should ask a human.
  private char recogniseImageCallback(byte[] pngData) {
    new ImageFrame(pngData).setVisible(true);
    return '?';
  }

  public static final void main(String[] args) throws IOException {
    BuildModel builder = new BuildModel();

    for (String path : args) {
      byte[] data = Files.toByteArray(new File(path));
      builder.recognise(data);
    }
  }
}
