"""Headless real-UDP integration test; run after building."""
import pathlib
import subprocess
import sys
import time

exe = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else "build/boss/unite_boss").resolve()
port = sys.argv[2] if len(sys.argv) > 2 else "17777"
host = subprocess.Popen([str(exe), "--host", "--headless", "--seconds", "5",
                         "--port", port], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
try:
    time.sleep(0.4)
    client = subprocess.run([str(exe), "--join", "127.0.0.1", "--headless",
                             "--seconds", "3", "--port", port],
                            capture_output=True, text=True, timeout=8)
    out, err = host.communicate(timeout=8)
    print("HOST:", out.strip())
    print("CLIENT:", client.stdout.strip())
    if host.returncode or client.returncode:
        raise RuntimeError(f"Process failed: {err} {client.stderr}")
    if "connected=1" not in out or "snapshot=1" not in client.stdout:
        raise RuntimeError("Connection / replication failed")
    x = float(client.stdout.split("archer_x=")[1].split()[0])
    if x <= 680:
        raise RuntimeError("Client input did not move the authoritative player")
    print("PASS: handshake, client input, host simulation, snapshots, clean shutdown")
finally:
    if host.poll() is None:
        host.terminate()
        host.wait(timeout=5)
