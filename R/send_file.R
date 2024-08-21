send_file <- function(filename,
                      title,
                      channel = Sys.getenv("CHANNEL_NAME"),
                      ts = NULL) {
  response <- POST(
    url = "https://slack.com/api/files.upload",
    add_headers(`Content-Type` = "multipart/form-data"),
    body = list(
      file = upload_file(filename),
      title = title,
      filename = filename,
      thread_ts = ts,
      token = slack_api_token,
      channels = channel
    )
  )
  content <- content(response)
  ts <- coalesce(ts,
                 content$file$shares$public[[names(content$file$shares$public)]][[1]]$ts,
                 content$file$shares$private[[names(content$file$shares$private)]][[1]]$ts)
  return(ts)
}