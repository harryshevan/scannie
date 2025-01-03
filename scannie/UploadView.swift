import SwiftUI
import UIKit
import Photos


struct UploadView: View {
    @Environment(\.presentationMode) var presentationMode
    @State private var updatedImage: UIImage?
    @State private var isLoading = true

    var originalImage: UIImage

    var body: some View {
        VStack {
            if self.isLoading {
                Text("Processing image...")
            } else if let updatedImage = updatedImage {
                Image(uiImage: updatedImage)
                    .resizable()
                    .scaledToFit()
                    .frame(maxWidth: 300, maxHeight: 300)
                Button("Save to Photo Library") {
                    saveImageToPhotoLibrary(image: updatedImage)
                }
                .padding()

            } else {
                Text("Failed to process image.")
            }
            Button("Back") {
                presentationMode.wrappedValue.dismiss()
            }
            .padding()
        }
        .onAppear(perform: processImage)
//        .navigationBarTitle("Image Upload", displayMode: .inline)
    }

    func processImage() {
        self.isLoading = true
        print("Enter processImage")

        // Convert UIImage to NSData
        guard let inputData = originalImage.pngData() else {
            print("Failed to convert original image to PNG data.")
            self.isLoading = false
            return
        }

        // Call the C function
        let inputBytes = [UInt8](inputData)
        var width: Int32 = 0
        var height: Int32 = 0
        var sz: Int32 = 0

        guard let processedImageData = MagicProcessImage(inputBytes, inputBytes.count, &width, &height, &sz) else {
            print("MagicProcessImage failed to return valid data.")
            self.isLoading = false
            return
        }

        defer {
            free(processedImageData)
        }

        print("Processed image dimensions: width = \(width), height = \(height)")

        // Ensure the data length matches expected size before creating UIImage
        if sz > 0 {
            let processedImage = Data(bytes: processedImageData, count: Int(sz))

            if let uiImage = UIImage(data: processedImage) {
                self.updatedImage = uiImage
            } else {
                print("Creating UIImage from processed data failed.")
            }
        } else {
            print("Processed image has invalid dimensions.")
        }

        self.isLoading = false
    }
}

func saveImageToPhotoLibrary(image: UIImage) {
    PHPhotoLibrary.requestAuthorization { status in
        if status == .authorized {
            UIImageWriteToSavedPhotosAlbum(image, nil, nil, nil)
        } else {
            print("Access to photo library is not authorized.")
        }
    }
}

