document.querySelector('input[type=file]').addEventListener('change', function() {
    // This is where you would handle the file POST request.
    console.log('A file was selected for upload:', this.files[0]);

    // Dummy function to emulate file upload
    setTimeout(() => {
        var li = document.createElement('li');
        li.textContent = this.files[0].name + ' (uploaded)';
        var deleteBtn = document.createElement('button');
        deleteBtn.textContent = 'Delete';
        deleteBtn.onclick = function() {
            // This is where you would handle the file DELETE request.
            console.log('A request to delete the file would be sent here.');
            li.parentNode.removeChild(li);
        };
        li.appendChild(deleteBtn);
        document.getElementById('fileList').appendChild(li);
    }, 1000); // Simulate a network delay
});