package id.dev.rikirg.downhillnative;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.graphics.Color;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.TextView;

import java.util.Locale;

public final class DiagnosticActivity extends Activity {
    private NativeSurface surface;
    private TextView telemetry;

    @Override
    public void onCreate(Bundle state) {
        super.onCreate(state);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        FrameLayout root = new FrameLayout(this);
        surface = new NativeSurface();
        root.addView(surface);
        telemetry = new TextView(this);
        telemetry.setTextColor(Color.rgb(232, 220, 180));
        telemetry.setTextSize(14);
        telemetry.setPadding(24, 18, 24, 18);
        telemetry.setShadowLayer(3f, 1f, 1f, Color.BLACK);
        root.addView(telemetry);
        setContentView(root);
    }

    @Override public void onPause() { surface.onPause(); super.onPause(); }
    @Override public void onResume() { super.onResume(); surface.onResume(); }

    private final class NativeSurface extends GLSurfaceView implements GLSurfaceView.Renderer {
        private float steer;
        private float throttle;
        private float brake;
        private long frames;
        private long windowStartNs;
        private float fps;
        private String inputSource = "Touch";

        NativeSurface() {
            super(DiagnosticActivity.this);
            setEGLContextClientVersion(3);
            setPreserveEGLContextOnPause(true);
            setRenderer(this);
            setRenderMode(RENDERMODE_CONTINUOUSLY);
            setFocusable(true);
            requestFocus();
        }

        @Override public void onSurfaceCreated(javax.microedition.khronos.opengles.GL10 gl, javax.microedition.khronos.egl.EGLConfig config) {
            nativeGlCreate();
            windowStartNs = System.nanoTime();
        }

        @Override public void onSurfaceChanged(javax.microedition.khronos.opengles.GL10 gl, int width, int height) {
            nativeGlResize(width, height);
        }

        @Override public void onDrawFrame(javax.microedition.khronos.opengles.GL10 gl) {
            nativeGlFrame(steer, throttle, brake);
            frames++;
            long now = System.nanoTime();
            if (now - windowStartNs >= 500_000_000L) {
                fps = frames * 1_000_000_000f / (now - windowStartNs);
                frames = 0;
                windowStartNs = now;
                post(() -> telemetry.setText(String.format(Locale.US,
                        "NATIVE RUNTIME DIAGNOSTIC\nFPS %.0f  |  %s\nSTEER %+.2f  THROTTLE %.2f  BRAKE %.2f\n\nTouch: left/right steer · top-right accelerate · bottom-right brake\nGamepad: left stick/D-pad steer · RT accelerate · LT brake",
                        fps, inputSource, steer, throttle, brake)));
            }
        }

        @Override public boolean onTouchEvent(MotionEvent event) {
            if (event.getActionMasked() == MotionEvent.ACTION_UP || event.getActionMasked() == MotionEvent.ACTION_CANCEL) {
                steer = throttle = brake = 0f;
                return true;
            }
            inputSource = "Touch";
            float x = event.getX() / Math.max(1f, getWidth());
            float y = event.getY() / Math.max(1f, getHeight());
            if (x < .5f) {
                steer = x < .25f ? -1f : 1f;
                throttle = brake = 0f;
            } else if (y < .5f) {
                throttle = 1f;
                brake = 0f;
            } else {
                brake = 1f;
                throttle = 0f;
            }
            return true;
        }

        @Override public boolean onGenericMotionEvent(MotionEvent event) {
            if ((event.getSource() & InputDevice.SOURCE_JOYSTICK) == InputDevice.SOURCE_JOYSTICK
                    && event.getAction() == MotionEvent.ACTION_MOVE) {
                inputSource = "Gamepad";
                steer = deadZone(event.getAxisValue(MotionEvent.AXIS_X));
                throttle = trigger(event, MotionEvent.AXIS_RTRIGGER, MotionEvent.AXIS_GAS);
                brake = trigger(event, MotionEvent.AXIS_LTRIGGER, MotionEvent.AXIS_BRAKE);
                return true;
            }
            return super.onGenericMotionEvent(event);
        }

        @Override public boolean onKeyDown(int keyCode, KeyEvent event) {
            if (isGamepad(event)) {
                inputSource = "Gamepad";
                applyGamepadKey(keyCode, true);
                return true;
            }
            return super.onKeyDown(keyCode, event);
        }

        @Override public boolean onKeyUp(int keyCode, KeyEvent event) {
            if (isGamepad(event)) {
                applyGamepadKey(keyCode, false);
                return true;
            }
            return super.onKeyUp(keyCode, event);
        }

        private void applyGamepadKey(int keyCode, boolean down) {
            float value = down ? 1f : 0f;
            if (keyCode == KeyEvent.KEYCODE_DPAD_LEFT) steer = down ? -1f : 0f;
            else if (keyCode == KeyEvent.KEYCODE_DPAD_RIGHT) steer = value;
            else if (keyCode == KeyEvent.KEYCODE_BUTTON_R1 || keyCode == KeyEvent.KEYCODE_BUTTON_A) throttle = value;
            else if (keyCode == KeyEvent.KEYCODE_BUTTON_L1 || keyCode == KeyEvent.KEYCODE_BUTTON_B) brake = value;
        }

        private boolean isGamepad(KeyEvent event) {
            return (event.getSource() & InputDevice.SOURCE_GAMEPAD) == InputDevice.SOURCE_GAMEPAD
                    || (event.getSource() & InputDevice.SOURCE_DPAD) == InputDevice.SOURCE_DPAD;
        }

        private float trigger(MotionEvent event, int primary, int fallback) {
            return Math.max(0f, Math.max(event.getAxisValue(primary), event.getAxisValue(fallback)));
        }

        private float deadZone(float value) { return Math.abs(value) < .15f ? 0f : value; }
    }

    private static native void nativeGlCreate();
    private static native void nativeGlResize(int width, int height);
    private static native void nativeGlFrame(float steer, float throttle, float brake);
}
