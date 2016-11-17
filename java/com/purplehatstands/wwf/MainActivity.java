package com.purplehatstands.wwf;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
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

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    final TextView view = new TextView(this);
    setContentView(view);
    view.setText("Thinking...");

    Intent intent = getIntent();

    if (Intent.ACTION_SEND.equals(intent.getAction())) {
      Uri imageUri = (Uri) intent.getParcelableExtra(Intent.EXTRA_STREAM);
      try {
        InputStream stream = getContentResolver().openInputStream(imageUri);
        byte[] bytes = ByteStreams.toByteArray(stream);
        ManagedChannel channel =
            ManagedChannelBuilder.forAddress("zaphod.purplehatstands.com", 32775)
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
}
