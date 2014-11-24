(function main() {
    var CANVAS_WIDTH = 800;
    var CANVAS_HEIGHT = 600;
    var ORIGINIMAGE_HEIGHT = 300;
    
    
    var fontSize = 72;
    var fontName = "NanumMyeongjo";
    var fontColor = "#FFF";
    var fileInput = document.getElementById("file-input");
    var textInput = document.getElementById("text-input");
    var submitButton = document.getElementById("submit-button");
    var content = document.getElementById("content");
    
    var avgR;
    var avgG;
    var avgB;
    var avgA;
    
    var inputImage = null;
    
    textInput.value = "달.";
    
    fileInput.addEventListener("change", function(){
        var file = this.files.item(0);
        var imageURL = URL.createObjectURL(file);
        inputImage = document.createElement("img");
        inputImage.src = imageURL;
        inputImage.height = ORIGINIMAGE_HEIGHT;
        content.appendChild(inputImage);
        
        
    }, false);
    
    submitButton.addEventListener("click", function(){
        if(inputImage === null){
            alert("no image");
        }
        
        else {
            getAvgRGBAFromImage(inputImage);
            
            var rCanvas = document.createElement("canvas");
            rCanvas.width = CANVAS_WIDTH;
            rCanvas.height = CANVAS_HEIGHT;
            var rCanvasCtx = rCanvas.getContext("2d"); 
            var rImageData = rCanvasCtx.createImageData(CANVAS_WIDTH, CANVAS_HEIGHT);
            for(var x = 0; x<CANVAS_WIDTH; ++x){
                for(var y = 0; y < CANVAS_HEIGHT; ++y){
                    setPixel(rImageData, x, y, avgR, avgG, avgB, avgA);
                }
            }
            rCanvasCtx.putImageData(rImageData, 0, 0);
            setText(rCanvasCtx, textInput.value, 10, 10, fontColor, fontName, fontSize);
            console.log(textInput.value);
            console.log(textInput);
            console.log(rCanvasCtx);
            
            content.appendChild(rCanvas);
        }
    }, false);
    
    function setPixel(imageData, x, y, r, g, b, a){
        var index = (x + y * imageData.width) * 4;
        imageData.data[index + 0] = r;
        imageData.data[index + 1] = g;
        imageData.data[index + 2] = b;
        imageData.data[index + 3] = a;   
    }
    function createCanvasFromImage(image){
        var rCanvas = document.createElement("canvas");
        rCanvas.width = image.naturalWidth;
        rCanvas.height = image.naturalHeight;
        var rCanvasCtx = rCanvas.getContext("2d");
        rCanvasCtx.drawImage(image, 0, 0);
        return rCanvas;
    }
    function setText(context, text, x, y, color, font, fontSize){
        context.fillStyle = "#FFF";
        context.font = fontSize + "px " + fontName;
        context.fillText(textInput.value, 200, fontSize + 150);
    }
    function getAvgRGBAFromImage(img){
        var imgCanvas = createCanvasFromImage(img);
        var imgCanvasCtx = imgCanvas.getContext("2d");
        var imgData = imgCanvasCtx.getImageData(0,0,imgCanvas.width, imgCanvas.height);
        var sumR = 0;
        var sumG = 0;
        var sumB = 0;
        var sumA = 0;
        var pixelNum = imgCanvas.width * imgCanvas.height;
        
        var idx = 0 ;
        for(var x = 0; x < imgCanvas.width; ++x){
            for(var y = 0; y < imgCanvas.height; ++y){
                idx = (x + y*imgCanvas.width) * 4;
                sumR += imgData.data[idx + 0];
                sumG += imgData.data[idx + 1];
                sumB += imgData.data[idx + 2];
                sumA += imgData.data[idx + 3];
            }
        }
        
        avgR = parseInt(sumR / pixelNum);
        avgG = parseInt(sumG / pixelNum);
        avgB = parseInt(sumB / pixelNum);
        avgA = parseInt(sumA / pixelNum);
        
    }
    
})();