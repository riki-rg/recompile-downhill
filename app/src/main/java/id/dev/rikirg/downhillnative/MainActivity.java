package id.dev.rikirg.downhillnative;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.provider.OpenableColumns;
import android.widget.Button;
import android.widget.TextView;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;

import java.io.IOException;
import java.io.InputStream;

public final class MainActivity extends AppCompatActivity {
    private static final long ISO_9660_PVD_OFFSET = 16L * IsoImageValidator.SECTOR_SIZE;

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
    }

    private void validateSelectedImage(Uri uri) {
        if (uri == null) {
            return;
        }

        String displayName = getDisplayName(uri);
        status.setText(getString(R.string.validating, displayName));

        try (InputStream input = getContentResolver().openInputStream(uri)) {
            if (input == null || !skipFully(input, ISO_9660_PVD_OFFSET)) {
                status.setText(R.string.iso_unreadable);
                return;
            }

            byte[] sector = input.readNBytes(IsoImageValidator.SECTOR_SIZE);
            switch (IsoImageValidator.validateSector(sector)) {
                case VALID:
                    getContentResolver().takePersistableUriPermission(uri, Intent.FLAG_GRANT_READ_URI_PERMISSION);
                    status.setText(getString(R.string.iso_valid, displayName));
                    break;
                case NOT_ISO_9660:
                    status.setText(R.string.iso_invalid);
                    break;
                case UNREADABLE:
                default:
                    status.setText(R.string.iso_unreadable);
                    break;
            }
        } catch (IOException | SecurityException exception) {
            status.setText(R.string.iso_unreadable);
        }
    }

    private static boolean skipFully(InputStream input, long bytes) throws IOException {
        long remaining = bytes;
        while (remaining > 0) {
            long skipped = input.skip(remaining);
            if (skipped == 0 && input.read() == -1) {
                return false;
            }
            remaining -= skipped == 0 ? 1 : skipped;
        }
        return true;
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
}
