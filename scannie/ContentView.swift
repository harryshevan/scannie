import SwiftUI


struct ContentView: View {
    @State private var image: UIImage? = nil
    @State private var isShowingImagePicker = false
    @State private var isShowingUploadView = false
    @State private var sourceType: UIImagePickerController.SourceType = .photoLibrary
    
    var body: some View


{
        NavigationView {
            VStack {
                if let image = image {
                    Image(uiImage: image)
                        .resizable()
                        .scaledToFit()
                        .frame(maxWidth: 300, maxHeight: 300)
                } else {
                    Text("Select an image to proceed")
                }
                
                HStack {
                    Button("Select Photo") {
                        self.sourceType = .photoLibrary
                        self.isShowingImagePicker = true
                    }
                    .padding()
                    
                    Button("Take Photo") {
                        self.sourceType = .camera
                        self.isShowingImagePicker = false
                    }
                    .padding()
                }
                
                if let image = image {
                    NavigationLink(destination: UploadView(originalImage: image)) {
                        Text("Scan!")
                    }
                }
            }
            .navigationBarTitle("Scannie")
        }
        .sheet(isPresented: $isShowingImagePicker) {
            ImagePicker(image: self.$image, sourceType: self.sourceType)
        }
    }
}

