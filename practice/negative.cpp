
void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void bubbleSort(vector<int>& nums) {
    int len = nums.size();
    for (int a = 0; a < len; a++) {
        for (int b = 0; b < len - a - 1; b++) {
            if (nums[b] > nums[b + 1]) {
                int swap = nums[b];
                nums[b] = nums[b + 1];
                nums[b + 1] = swap;
            }
        }
    }
}
