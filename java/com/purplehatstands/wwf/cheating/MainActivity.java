package com.purplehatstands.wwf.cheating;

import android.app.Activity;
import android.content.Intent;
import android.database.DataSetObserver;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.GridView;
import android.widget.LinearLayout;
import android.widget.ListAdapter;
import android.widget.ProgressBar;
import android.widget.TextView;
import com.google.common.io.ByteStreams;
import com.google.protobuf.ByteString;
import java.io.IOException;
import java.io.InputStream;
import words.Messages.Response;

public class MainActivity extends Activity {
  private static final String TAG = "MainActivity";

  private static final int GRID_SIZE = 15;

  private LinearLayout layout;
  private TextView view;
  private GridView gridView;
  private ProgressBar progress;
  private Grid grid;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.main);
    this.view = (TextView) findViewById(R.id.text);
    this.gridView = (GridView) findViewById(R.id.grid);
    this.progress = (ProgressBar) findViewById(R.id.progress);
    grid = new Grid(getAssets());
  }

  @Override
  public void onResume() {
    super.onResume();
    Intent intent = getIntent();

    Log.d(TAG, "Resuming: " + intent);

    if (Intent.ACTION_SEND.equals(intent.getAction())) {
      Uri imageUri = (Uri) intent.getParcelableExtra(Intent.EXTRA_STREAM);
      new AsyncTask<Uri, Void, Response>() {
        @Override
        protected Response doInBackground(Uri... uris) {
          Uri uri = uris[0];
          try {
            InputStream stream = getContentResolver().openInputStream(uri);
            byte[] bytes = ByteStreams.toByteArray(stream);
            return grid.solve(getAssets(), bytes);
          } catch (IOException e) {
            throw new RuntimeException(e);
          }
        }

        @Override
        protected void onPostExecute(Response result) {
          progress.setVisibility(View.GONE);
          view.setText(result.toString());
          renderBoard(result);
        }
      }.execute(imageUri);
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

  private void renderBoard(final Response response) {
    final Response.Solution bestSolution = getBestSolution(response);
    final ByteString data = response.getBoard().getData();
    gridView.setNumColumns(GRID_SIZE);
    gridView.setAdapter(
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
  }
}
