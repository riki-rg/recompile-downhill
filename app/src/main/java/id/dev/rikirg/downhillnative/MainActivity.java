package id.dev.rikirg.downhillnative;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.ParcelFileDescriptor;
import android.provider.OpenableColumns;
import android.widget.Button;
import android.widget.TextView;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;

import java.io.IOException;

public final class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("downhillnative");
    }

    private TextView status;

    private final ActivityResultLauncher<String[]> isoPicker = registerForActivityResult(
            new ActivityResultContracts.OpenDocument(), this::validateSelectedImage);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        status = findViewById(R.id.status);
        TextView runtime = findViewById(R.id.runtime);
        runtime.setText(getString(R.string.native_runtime, nativeRuntimeName()));

        Button selectIso = findViewById(R.id.select_iso);
        selectIso.setOnClickListener(view -> isoPicker.launch(new String[]{"application/octet-stream", "application/x-iso9660-image"}));

        Button diagnostic = findViewById(R.id.open_diagnostic);
        diagnostic.setOnClickListener(view -> startActivity(new Intent(this, DiagnosticActivity.class)));
    }

    private void validateSelectedImage(Uri uri) {
        if (uri == null) {
            return;
        }

        String displayName = getDisplayName(uri);
        status.setText(getString(R.string.validating, displayName));

        try (ParcelFileDescriptor descriptor = getContentResolver().openFileDescriptor(uri, "r")) {
            if (descriptor == null) {
                status.setText(R.string.iso_unreadable);
                return;
            }
            getContentResolver().takePersistableUriPermission(uri, Intent.FLAG_GRANT_READ_URI_PERMISSION);
            status.setText(getString(R.string.iso_report, displayName, nativeExploreIso(descriptor.getFd())));
        } catch (IOException | SecurityException exception) {
            status.setText(R.string.iso_unreadable);
        }
    }

    private String getDisplayName(Uri uri) {
        try (android.database.Cursor cursor = getContentResolver().query(uri, null, null, null, null)) {
            if (cursor != null && cursor.moveToFirst()) {
                int index = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME);
                if (index >= 0) {
                    return cursor.getString(index);
                }
            }
        }
        return getString(R.string.unknown_file);
    }

    private static native String nativeRuntimeName();

    private static native String nativeExploreIso(int fileDescriptor);
}
