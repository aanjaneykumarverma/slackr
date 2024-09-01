
# `slackr` 

`slackr` is an R package that provides a set of functions to interact with Slack APIs seamlessly through R. Whether you need to send messages, upload files, or manage Slack channels and users, `slackr` makes it easy to integrate Slack's functionality directly into your R workflows.

## Features

- **Send Messages**: Post messages to specific Slack channels or direct messages to users.
- **Upload Files**: Upload and share files, images, and plots directly from R to your Slack workspace.
- **Manage Channels**: Create, archive, and manage channels.
- **User Management**: Retrieve user information, invite users to channels, and manage user interactions.
- **API Integration**: Provides an easy-to-use wrapper around Slack's Web API, with support for various endpoints.

## Installation

You can install the `slackr` package from GitHub using the `devtools` package:

```
# Install devtools if you haven't already
install.packages("devtools")

# Install slackr
devtools::install_github("yourusername/slackr")
```

## Usage

First, you need to configure your Slack API tokens and workspace information. These are typically stored as environment variables.

```
# Set your Slack API token (replace with your actual token)
Sys.setenv(SLACK_API_TOKEN = "your-slack-api-token")
```

## Sending a Message
```
library(slackr)
# Send a message to a channel
send_text(header = "Slack Alert", body_text = "Hello world!", channel = "<channel_id/channel_name here>")
```

## Contributing

We welcome contributions to enhance the functionality of the `slackr` package. Please feel free to submit pull requests, report issues, or suggest new features on our [GitHub repository](https://github.com/aanjaneykumarverma/slackr).

## License

This package is licensed under the MIT License. See the [LICENSE](https://github.com/aanjaneykumarverma/slackr/blob/main/LICENSE) file for more details.
