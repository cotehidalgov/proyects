class User < ApplicationRecord

	has_secure_password
	has_attached_file :avatar, styles: { medium: '300x300>', thumb: '100x100>' }, default_url: 'http://s3.amazonaws.com/37assets/svn/765-default-avatar.png'
	validates_attachment_content_type :avatar, content_type: /\Aimage\/.*\z/

	has_many :likes
	has_many :donations
	has_many :proyects, :dependent => :destroy
	has_many :proyects, as: :liked_proyects, :through => :likes
	has_many :comments, :dependent => :destroy

	validates :password, presence: true, length: {minimum: 6},
						confirmation: true, allow_blank: false
	validates :mail, presence: true, uniqueness: true, allow_blank: false,
						format: /\A([^@\s]+)@((?:[-a-z0-9]+\.)+[a-z]{2,})\z/i
	validates :username, uniqueness: true

end
