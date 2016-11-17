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
import com.google.common.util.concurrent.FutureCallback;
import com.google.common.util.concurrent.Futures;
import com.google.common.util.concurrent.ListenableFuture;
import com.google.protobuf.ByteString;
import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import java.io.IOException;
import java.io.InputStream;
import words.CheaterGrpc;
import words.Service.Request;
import words.Service.Response;

public class MainActivity extends Activity {
  private static final String TAG = "MainActivity";

  private static final int GRID_SIZE = 15;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    final LinearLayout layout = new LinearLayout(this);
    layout.setOrientation(LinearLayout.VERTICAL);
    setContentView(layout);

    final TextView view = new TextView(this);
    layout.addView(view);
    view.setText("Thinking...");

    Intent intent = getIntent();

    if (Intent.ACTION_SEND.equals(intent.getAction())) {
      Uri imageUri = (Uri) intent.getParcelableExtra(Intent.EXTRA_STREAM);
      try {
        InputStream stream = getContentResolver().openInputStream(imageUri);
        byte[] bytes = ByteStreams.toByteArray(stream);
        ManagedChannel channel =
            ManagedChannelBuilder.forAddress("zaphod.purplehatstands.com", 32776)
                .usePlaintext(true)
                .build();
        CheaterGrpc.CheaterFutureStub stub = CheaterGrpc.newFutureStub(channel);
        Request request = Request.newBuilder().setImage(ByteString.copyFrom(bytes)).build();
        ListenableFuture<Response> response = stub.findSolutions(request);
        Futures.addCallback(
            response,
            new FutureCallback<Response>() {
              @Override
              public void onSuccess(final Response response) {
                Log.d(TAG, "Response:" + response);
                runOnUiThread(
                    new Runnable() {
                      @Override
                      public void run() {
                        view.setText(response.toString());
                        GridView grid = renderBoard(response);
                        layout.addView(grid);
                      }
                    });
              }

              @Override
              public void onFailure(Throwable throwable) {
                Log.d(TAG, "Failed: " + throwable);
              }
            });
      } catch (IOException e) {
        e.printStackTrace();
      }
    }
  }

  private GridView renderBoard(final Response response) {
    final Response.Solution bestSolution = response.getSolution(0);
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
