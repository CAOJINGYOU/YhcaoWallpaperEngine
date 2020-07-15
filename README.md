分类

1. 单图
2. 多图
1. bing
1. unsplash
1. 网页
1. 视频

类型

1. SingleImage
1. MultipleImage
1. Bing
1. Unsplash
1. Web
1. Video

默认json配置

	{
		"selecttype": "SingleImage",
		"SingleImage": {
			"imagepath": "Yasuo.jpg"
		},
		"MultipleImage": {
			"imagepath": "",
			"time": 60
		},
		"Bing": {},
		"Unsplash": {
			"time": 60,
			"selectindex": 0,
			"url": [
				"https://source.unsplash.com/random",
				"https://source.unsplash.com/user/yhcao",
				"https://source.unsplash.com/user/yhcao/likes",
				"https://source.unsplash.com/weekly?water"
			],
		},
		"Web": {
			"url": ""
		},
		"Video": {
			"videopath": ""
		}
	}