from flask import Flask, render_template, request, url_for
import cv2
import numpy as np
import os
import subprocess

app = Flask(__name__)

UPLOAD_FOLDER = "static/uploads"
OUTPUT_FOLDER = "static/outputs"

os.makedirs(UPLOAD_FOLDER, exist_ok=True)
os.makedirs(OUTPUT_FOLDER, exist_ok=True)

@app.route("/", methods=["GET", "POST"])
def index():
    if request.method == "POST":
        file = request.files["image"]

        if file:
            filename = file.filename
            upload_path = os.path.join(UPLOAD_FOLDER, filename)
            file.save(upload_path)

            # ✅ FIX 1: Check image
            img = cv2.imread(upload_path)
            if img is None:
                return "Error: Image not loaded properly"

            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
            h, w = gray.shape

            # Save shape
            with open("static/outputs/shape.txt", "w") as f:
                f.write(f"{h} {w}")

            # Save CSV
            np.savetxt("static/outputs/pixels.csv", gray, fmt="%d", delimiter=",")

            # ✅ FIX 2: Windows needs .exe
            result = subprocess.run(["huffman.exe"], capture_output=True, text=True)

            print(result.stdout)
            print(result.stderr)

            # ✅ FIX 3: Check decoded file
            decoded_path = "static/outputs/decoded.txt"
            if not os.path.exists(decoded_path):
                return "Error: Huffman did not generate decoded.txt"

            # Read decoded pixels
            pixels = list(map(int, open(decoded_path).read().split()))

            # ✅ FIX 4: Check size
            if len(pixels) != h * w:
                return "Error: Pixel size mismatch"

            # Reconstruct image
            image = np.array(pixels, dtype=np.uint8).reshape((h, w))

            output_filename = "reconstructed.jpg"
            output_path = os.path.join(OUTPUT_FOLDER, output_filename)

            cv2.imwrite(output_path, image)

            return render_template("index.html",
                                   original=filename,
                                   output=output_filename)

    return render_template("index.html")

if __name__ == "__main__":
    app.run(debug=True)