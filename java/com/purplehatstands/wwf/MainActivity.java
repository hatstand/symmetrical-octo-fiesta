package com.purplehatstands.wwf;

import android.app.Activity;
import android.content.Intent;
import android.database.DataSetObserver;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.GridView;
import android.widget.LinearLayout;
import android.widget.ListAdapter;
import android.widget.TextView;
import com.google.common.io.ByteStreams;
import com.google.protobuf.ByteString;
import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import words.Service.Response;

public class MainActivity extends Activity {
  private static final String TAG = "MainActivity";

  private static final int GRID_SIZE = 15;

  private LinearLayout layout;
  private TextView view;
  private Grid grid;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    layout = new LinearLayout(this);
    layout.setOrientation(LinearLayout.VERTICAL);
    setContentView(layout);

    view = new TextView(this);
    layout.addView(view);
    view.setText("Thinking...");

    grid = new Grid(getAssets());
  }

  @Override
  public void onResume() {
    super.onResume();
    Intent intent = getIntent();

    Log.d(TAG, "Resuming: " + intent);

    if (Intent.ACTION_SEND.equals(intent.getAction())) {
      Uri imageUri = (Uri) intent.getParcelableExtra(Intent.EXTRA_STREAM);
      try {
        InputStream stream = getContentResolver().openInputStream(imageUri);
        byte[] bytes = ByteStreams.toByteArray(stream);
        view.setText(new String(recogniseGrid(bytes), StandardCharsets.UTF_8));

        byte[] grid = recogniseGrid(bytes);
        byte[] rack = recogniseRack(bytes);
        this.grid.solve(getAssets(), bytes);
      } catch (IOException e) {
        e.printStackTrace();
      }
    }
  }

  private byte[] recogniseGrid(byte[] bytes) {
    return grid.recogniseGrid(getAssets(), bytes);
  }

  private byte[] recogniseRack(byte[] bytes) {
    return grid.recogniseRack(getAssets(), bytes);
  }

  private Response.Solution getBestSolution(Response response) {
    Response.Solution ret = null;
    int score = 0;
    for (Response.Solution solution : response.getSolutionList()) {
      if (solution.getScore() > score) {
        score = solution.getScore();
        ret = solution;
      }
    }
    return ret;
  }

  private GridView renderBoard(final Response response) {
    final Response.Solution bestSolution = getBestSolution(response);
    final ByteString data = response.getBoard().getData();
    GridView grid = new GridView(this);
    grid.setNumColumns(GRID_SIZE);
    grid.setAdapter(
        new ListAdapter() {
          @Override
          public int getCount() {
            return data.size();
          }

          @Override
          public boolean areAllItemsEnabled() {
            return true;
          }

          @Override
          public boolean isEnabled(int position) {
            return true;
          }

          @Override
          public Object getItem(int position) {
            if (!isSolution(position)) {
              return (char) data.byteAt(position);
            }

            int x = position % GRID_SIZE;
            int y = position / GRID_SIZE;

            if (bestSolution.getDirection().equals(Response.Solution.Direction.ROW)) {
              return bestSolution.getWord().charAt(x - bestSolution.getX());
            } else {
              return bestSolution.getWord().charAt(y - bestSolution.getY());
            }
          }

          @Override
          public long getItemId(int position) {
            return position / GRID_SIZE;
          }

          @Override
          public int getItemViewType(int position) {
            return 0;
          }

          private boolean isSolution(int position) {
            int x = position % GRID_SIZE;
            int y = position / GRID_SIZE;
            if (bestSolution.getDirection().equals(Response.Solution.Direction.ROW)) {
              return x >= bestSolution.getX()
                  && x < (bestSolution.getX() + bestSolution.getWord().length())
                  && bestSolution.getY() == y;
            } else {
              return y >= bestSolution.getY()
                  && y < (bestSolution.getY() + bestSolution.getWord().length())
                  && bestSolution.getX() == x;
            }
          }

          @Override
          public View getView(int position, View convertView, ViewGroup parent) {
            String text = Character.toString((char) getItem(position));
            TextView view =
                convertView != null ? (TextView) convertView : new TextView(MainActivity.this);
            view.setText(text);
            if (isSolution(position)) {
              view.setTextColor(0xffff0000);
            } else {
              view.setTextColor(0xffffffff);
            }
            return view;
          }

          @Override
          public int getViewTypeCount() {
            return 1;
          }

          @Override
          public boolean hasStableIds() {
            return true;
          }

          @Override
          public boolean isEmpty() {
            return false;
          }

          @Override
          public void registerDataSetObserver(DataSetObserver observer) {}

          @Override
          public void unregisterDataSetObserver(DataSetObserver observer) {}
        });
    return grid;
  }
}
